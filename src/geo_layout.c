#include "geo_layout.h"
#include "src/geo_node.h"
#include "src/matrix.h"
#include "src/util.h"

static GeoLayoutCommandProc GeoLayoutJumpTable[] = {
    geo_layout_cmd_branch_and_link,
    geo_layout_cmd_end,
    geo_layout_cmd_jump,
    geo_layout_cmd_branch,
    geo_layout_cmd_return,
    geo_layout_cmd_open_node,
    geo_layout_cmd_close_node,
    geo_layout_cmd_attach_node,
    geo_layout_cmd_set_callback,
    geo_layout_cmd_create_container,
    geo_layout_cmd_create_reference,
    geo_layout_cmd_create_camera,
    geo_layout_cmd_create_type3,
    geo_layout_cmd_create_type4,
    geo_layout_cmd_create_ortho,
    geo_layout_cmd_create_perspective,
    geo_layout_cmd_create_background,
    geo_layout_cmd_create_clear_depth,
    geo_layout_cmd_noop,
    geo_layout_cmd_create_fog,
    geo_layout_cmd_create_light,
    geo_layout_cmd_skip,
    geo_layout_cmd_create_ambient_light,
    geo_layout_cmd_create_shadow_context,
    geo_layout_cmd_create_shadow,
    geo_layout_cmd_create_cull_distance,
    geo_layout_cmd_create_switch_case,
    geo_layout_cmd_create_translate_rotate,
    geo_layout_cmd_create_translate,
    geo_layout_cmd_create_animated_part,
    geo_layout_cmd_create_display_list_part,
    geo_layout_cmd_create_model_part,
    geo_layout_cmd_create_display_list_matrix,
    geo_layout_cmd_create_scale,
    geo_layout_cmd_create_display_list,
    geo_layout_cmd_create_shadow_texture,
    geo_layout_cmd_create_anchor,
    geo_layout_cmd_set_node_flag4,
    geo_layout_cmd_create_group,
};

static MemoryBlock* gGraphNodePool;
static GraphNode* gCurRootGraphNode;
static uintptr_t gGeoLayoutStack[16]; // PORT64: holds return addresses (was u32)
static GraphNode* gCurGraphNodeList[32];
static s16 gCurGraphNodeIndex;
static s16 gGeoLayoutStackIndex;
static s16 D_800ABDFC;
static s16 gGeoLayoutReturnIndex;
static u8* gGeoLayoutCommand;
int gGeoPacked;
extern int Port_IsRawData(const void* p);

static void GeoSetCmd(u8* p) {
    gGeoLayoutCommand = p;
    gGeoPacked = (p != NULL) && Port_IsRawData(p);
}

/*
  0x00: Branch and store return address
   cmd+0x04: void *branchTarget
*/
void geo_layout_cmd_branch_and_link(void) {
    u8* cmd = (void*)gGeoLayoutCommand;

    cmd += 0;

    gGeoLayoutStack[gGeoLayoutStackIndex++] = (uintptr_t)(cmd + GL_CMDSIZE(8));
    gGeoLayoutStack[gGeoLayoutStackIndex++] = (gCurGraphNodeIndex << 16) + gGeoLayoutReturnIndex;
    gGeoLayoutReturnIndex = gGeoLayoutStackIndex;
    GeoSetCmd(Util_ConvertAddrToVirtAddr((uintptr_t)cur_geo_cmd_ptr(0x04)));
}

// 0x01: Terminate geo layout
void geo_layout_cmd_end(void) {
    gGeoLayoutStackIndex = gGeoLayoutReturnIndex;
    gGeoLayoutReturnIndex = gGeoLayoutStack[--gGeoLayoutStackIndex] & 0xFFFF;
    gCurGraphNodeIndex = gGeoLayoutStack[gGeoLayoutStackIndex] >> 16;
    GeoSetCmd((u8*)gGeoLayoutStack[--gGeoLayoutStackIndex]);
}

/*
  0x02: Jump to address
    cmd+0x04: void *jumpTarget
*/
void geo_layout_cmd_jump(void) {
    u8* cmd = (void*)gGeoLayoutCommand;

    cmd += 0;

    GeoSetCmd(Util_ConvertAddrToVirtAddr((uintptr_t)cur_geo_cmd_ptr(0x04)));
}

/*
  0x03: Branch
    cmd+0x04: void *branchTarget
*/
void geo_layout_cmd_branch(void) {
    u8* cmd = (void*)gGeoLayoutCommand;

    cmd += 0;

    // unlike in sm64, this argument doesnt seem to be optional.
    gGeoLayoutStack[gGeoLayoutStackIndex++] = (uintptr_t)(cmd + GL_CMDSIZE(8));
    GeoSetCmd(Util_ConvertAddrToVirtAddr((uintptr_t)cur_geo_cmd_ptr(0x04)));
}

// 0x04: Return from branch
void geo_layout_cmd_return(void) {
    GeoSetCmd((u8*)gGeoLayoutStack[--gGeoLayoutStackIndex]);
}

// 0x05: Open node
void geo_layout_cmd_open_node(void) {
    gCurGraphNodeList[gCurGraphNodeIndex + 1] = gCurGraphNodeList[gCurGraphNodeIndex];
    gCurGraphNodeIndex++;
    gGeoLayoutCommand += GL_CMDSIZE(0x04);
}

// 0x06: Close node
void geo_layout_cmd_close_node(void) {
    gCurGraphNodeIndex--;
    gGeoLayoutCommand += GL_CMDSIZE(0x04);
}

void register_graph_node(GraphNode* arg0) {
    if (arg0 != NULL) {
        gCurGraphNodeList[gCurGraphNodeIndex] = arg0;
        if (gCurGraphNodeIndex == 0) {
            if (gCurRootGraphNode == NULL) {
                gCurRootGraphNode = arg0;
            }
        } else {
            GraphNode_AppendChild(gCurGraphNodeList[gCurGraphNodeIndex - 1], arg0);
        }
    }
}

void geo_layout_cmd_attach_node(void) {

    register_graph_node(&((unk_D_8690A610*)Util_ConvertAddrToVirtAddr((uintptr_t)GL_PTR(0x4)))->node);

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_set_callback(void) {

    if (gCurGraphNodeList[gCurGraphNodeIndex] != NULL) {
        GeoNode_SetCallback(gCurGraphNodeList[gCurGraphNodeIndex], ((GraphNodeCallback)GL_PTR(0x4)), ((unk_D_86002F34_000_014*)GL_PTR(0x8)));
    }

    gGeoLayoutCommand += GL_CMDSIZE(0xc);
}

void geo_layout_cmd_create_container(void) {
    register_graph_node(GeoNode_CreateContainer(gGraphNodePool, NULL));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_reference(void) {

    register_graph_node(GeoNode_CreateWithReference(gGraphNodePool, NULL, ((GraphNode*)Util_ConvertAddrToVirtAddr((uintptr_t)GL_PTR(0x4)))));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_camera(void) {
    unk_D_86002F34_00C* temp_v0 =
        GeoNode_CreateCamera(gGraphNodePool, NULL, ((u16)GL_S16(0x4)), ((u16)GL_S16(0x6)), GL_S16(0x8), GL_S16(0xa));

    if (temp_v0 != NULL) {
        temp_v0->unk_24.fovy = ((u16)GL_S16(0x2));
        Vec3f_FromVec3s(&temp_v0->unk_60.at, (&(Vec3s){ GL_S16(0xc), GL_S16(0xe), GL_S16(0x10) }));
        Camera_ComputeEyeFromAngles(&temp_v0->unk_60.at, &temp_v0->unk_60.eye, GL_S16(0x16), (GL_S16(0x12) << 0xF) / 180,
                      (GL_S16(0x14) << 0xF) / 180);
    }

    register_graph_node(&temp_v0->node);

    gGeoLayoutCommand += GL_CMDSIZE(0x18);
}

void geo_layout_cmd_create_type3(void) {

    register_graph_node(GeoNode_CreateType3(gGraphNodePool, NULL, GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_type4(void) {

    register_graph_node(GeoNode_CreateType4(gGraphNodePool, NULL, GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_ortho(void) {

    register_graph_node(GeoNode_CreateOrtho(gGraphNodePool, NULL, GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_perspective(void) {

    register_graph_node(GeoNode_CreatePerspective(gGraphNodePool, NULL, GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_background(void) {

    register_graph_node(GeoNode_CreateBackground(gGraphNodePool, NULL, GL_U8(0x1), GL_U8(0x2), GL_U8(0x3)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_clear_depth(void) {
    register_graph_node(GeoNode_CreateClearDepth(gGraphNodePool, NULL));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_noop(void) {
}

void geo_layout_cmd_create_fog(void) {

    register_graph_node(GeoNode_CreateFog(gGraphNodePool, NULL, GL_S16(0x4), GL_S16(0x6), GL_U8(0x1), GL_U8(0x2), GL_U8(0x3), 0xFF));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_light(void) {

    register_graph_node(GeoNode_CreateLight(gGraphNodePool, NULL, (GL_S16(0x4) << 0xF) / 180, (GL_S16(0x6) << 0xF) / 180, GL_U8(0x8),
                                GL_U8(0x9), GL_U8(0xa), GL_U8(0xb)));

    gGeoLayoutCommand += GL_CMDSIZE(0xc);
}

void geo_layout_cmd_skip(void) {
    gGeoLayoutCommand += GL_CMDSIZE(0xc);
}

void geo_layout_cmd_create_ambient_light(void) {

    register_graph_node(GeoNode_CreateAmbientLight(gGraphNodePool, NULL, GL_U8(0x1), GL_U8(0x2), GL_U8(0x3)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_shadow_context(void) {

    register_graph_node(GeoNode_CreateShadowContext(gGraphNodePool, NULL, GL_S16(0x2), ((unk_D_86002F34_018*)GL_PTR(0x8)), GL_S16(0x4), ((unk_D_86002F34_01C*)GL_PTR(0xc)), GL_S16(0x6),
                                ((Vtx*)GL_PTR(0x10))));

    gGeoLayoutCommand += GL_CMDSIZE(0x14);
}

void geo_layout_cmd_create_shadow(void) {

    register_graph_node(GeoNode_CreateShadow(gGraphNodePool, NULL, GL_U8(0x1), GL_S16(0x4), GL_S16(0x6), GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_cull_distance(void) {

    register_graph_node(GeoNode_CreateCullDistance(gGraphNodePool, NULL, GL_S16(0x4), GL_S16(0x6)));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_switch_case(void) {

    register_graph_node(GeoNode_CreateSwitchCase(gGraphNodePool, NULL, GL_U8(0x1), GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_translate_rotate(void) {
    Vec3s sp30;
    Vec3f sp24;
    UNUSED s32 pad;

    Vec3f_FromVec3s(&sp24, (&(Vec3s){ GL_S16(0xa), GL_S16(0xc), GL_S16(0xe) }));
    Vec3s_SetComponents(&sp30, (GL_S16(0x4) << 0xF) / 180, (GL_S16(0x6) << 0xF) / 180, (GL_S16(0x8) << 0xF) / 180);
    register_graph_node(GeoNode_CreateTranslateRotate(gGraphNodePool, NULL, &sp24, &sp30));

    gGeoLayoutCommand += GL_CMDSIZE(0x10);
}

void geo_layout_cmd_create_translate(void) {
    Vec3f sp24;

    sp24.x = GL_U32(0x4) / 65536.0f;
    sp24.y = GL_U32(0x8) / 65536.0f;
    sp24.z = GL_U32(0xc) / 65536.0f;
    register_graph_node(GeoNode_CreateTranslate(gGraphNodePool, NULL, &sp24));

    gGeoLayoutCommand += GL_CMDSIZE(0x10);
}

void geo_layout_cmd_create_animated_part(void) {
    Vec3f sp3C;
    s16 var_a3 = 1;

    sp3C.x = GL_U32(0x10) / 65536.0f;
    sp3C.y = GL_U32(0x14) / 65536.0f;
    sp3C.z = GL_U32(0x18) / 65536.0f;

    if (GL_U8(0x2) & 1) {
        var_a3 = 0;
    }

    if (GL_U8(0x2) & 2) {
        var_a3 |= 2;
    }

    register_graph_node(
        GeoNode_CreateAnimatedPart(gGraphNodePool, NULL, GL_U8(0x1), var_a3, GL_U8(0x3), (&(Vec3s){ GL_S16(0x4), GL_S16(0x6), GL_S16(0x8) }), (&(Vec3s){ GL_S16(0xa), GL_S16(0xc), GL_S16(0xe) }), &sp3C));

    gGeoLayoutCommand += GL_CMDSIZE(0x1c);
}

void geo_layout_cmd_create_display_list_part(void) {

    register_graph_node(GeoNode_CreateDisplayListPart(gGraphNodePool, NULL, GL_U8(0x1), ((Gfx*)GL_PTR(0x4)), GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_model_part(void) {
    Vec3s sp40;
    Vec3f sp34;
    Vec3f sp28;
    DisplayObject* tmp;

    Vec3f_FromVec3s(&sp34, (&(Vec3s){ GL_S16(0xa), GL_S16(0xc), GL_S16(0xe) }));

    sp28.x = GL_S16(0x10) / 100.0f;
    sp28.y = GL_S16(0x12) / 100.0f;
    sp28.z = GL_S16(0x14) / 100.0f;

    Vec3s_SetComponents(&sp40, (GL_S16(0x4) << 0xF) / 180, (GL_S16(0x6) << 0xF) / 180, (GL_S16(0x8) << 0xF) / 180);
    tmp = GeoNode_CreateModelPart(gGraphNodePool, NULL, GL_S16(0x2), &sp34, &sp40, &sp28);
    register_graph_node(tmp);

    gGeoLayoutCommand += GL_CMDSIZE(0x18);
}

void geo_layout_cmd_create_display_list_matrix(void) {
    Vec3f sp34;
    Vec3s sp2C;
    unk_D_86002F34_alt8* tmp;

    Vec3f_FromVec3s(&sp34, (&(Vec3s){ GL_S16(0xa), GL_S16(0xc), GL_S16(0xe) }));
    Vec3s_SetComponents(&sp2C, (GL_S16(0x4) << 0xF) / 180, (GL_S16(0x6) << 0xF) / 180, (GL_S16(0x8) << 0xF) / 180);
    tmp = GeoNode_CreateDisplayListMatrixFromTransform(gGraphNodePool, NULL, GL_U8(0x1), ((Gfx*)GL_PTR(0x10)), &sp34, &sp2C);
    register_graph_node(tmp);

    gGeoLayoutCommand += GL_CMDSIZE(0x14);
}

void geo_layout_cmd_create_scale(void) {
    Vec3f sp2C;
    unk_D_86002F34_alt9* tmp;

    Vec3f_FromVec3s(&sp2C, (&(Vec3s){ GL_S16(0x2), GL_S16(0x4), GL_S16(0x6) }));
    tmp = GeoNode_CreateScale(gGraphNodePool, NULL, GL_U8(0x1), ((Gfx*)GL_PTR(0xc)), &sp2C, GL_U32(0x8) / 65536.0f);
    register_graph_node(tmp);

    gGeoLayoutCommand += GL_CMDSIZE(0x10);
}

void geo_layout_cmd_create_display_list(void) {

    register_graph_node(GeoNode_CreateDisplayList(gGraphNodePool, NULL, GL_U8(0x1), ((Gfx*)GL_PTR(0x4))));

    gGeoLayoutCommand += GL_CMDSIZE(0x8);
}

void geo_layout_cmd_create_shadow_texture(void) {

    register_graph_node(GeoNode_CreateShadowTexture(gGraphNodePool, NULL, GL_U8(0x1), GL_S16(0x2), ((Gfx*)GL_PTR(0x4)), GL_S16(0x8), GL_S16(0xa),
                                GL_U8(0xc), GL_U8(0xd), GL_U8(0xe), GL_U8(0xf)));

    gGeoLayoutCommand += GL_CMDSIZE(0x10);
}

void geo_layout_cmd_create_anchor(void) {

    register_graph_node(GeoNode_CreateAnchor(gGraphNodePool, NULL, GL_S16(0x2)));

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_set_node_flag4(void) {

    if (gCurGraphNodeList[gCurGraphNodeIndex] != NULL) {
        gCurGraphNodeList[gCurGraphNodeIndex]->renderFlags |= 4;
    }

    gGeoLayoutCommand += GL_CMDSIZE(0x4);
}

void geo_layout_cmd_create_group(void) {

    register_graph_node(GeoNode_CreateGroup(gGraphNodePool, NULL, GL_U8(0x1), GL_S16(0x4), GL_S16(0x6), GL_S16(0x8), GL_S16(0xa),
                                GL_U8(0x2), ((s8)GL_U8(0x3)), ((s32)GL_U32(0xc)), GL_U8(0x10), GL_U8(0x11), GL_U8(0x12), GL_U8(0x13)));

    gGeoLayoutCommand += GL_CMDSIZE(0x14);
}

#include <stdio.h>
#include <stdlib.h>
static void Port_DumpGraph(GraphNode* n, int depth) {
    GraphNode* c;
    if (n == NULL) return;
    fprintf(stderr, "[geo] %*s%p type=%d flags=0x%x cb=%p children=%p next=%p\n", depth * 2, "", (void*)n, n->type, n->flags, (void*)n->callback, (void*)n->children, (void*)n->nextSibling);
    c = (GraphNode*)n->children;
    if (c != NULL && depth < 12) {
        GraphNode* it = c;
        do {
            Port_DumpGraph(it, depth + 1);
            it = it->nextSibling;
        } while (it != c && it != NULL);
    }
}

GraphNode* process_geo_layout(MemoryBlock* pool, void* segptr) {
    // set by register_scene_graph_node when gCurGraphNodeIndex is 0
    // and gCurRootGraphNode is NULL
    gCurRootGraphNode = NULL;

    // gGeoNumViews seems to have been removed in this version of geo_script.c
    gCurGraphNodeIndex = 0;

    gGeoLayoutStackIndex = 2;
    gGeoLayoutReturnIndex = 2; // stack index is often copied here?

    GeoSetCmd((u8*)Util_ConvertAddrToVirtAddr((uintptr_t)segptr));

    gGraphNodePool = pool;

    gCurGraphNodeList[0] = 0;

    gGeoLayoutStack[0] = 0;
    gGeoLayoutStack[1] = 0;

    while (gGeoLayoutCommand != NULL) {
        if (getenv("PORT_TRACE_GEO")) fprintf(stderr, "[geo-cmd] %p op=0x%x idx=%d\n", (void*)gGeoLayoutCommand, GL_U8(0x00), gCurGraphNodeIndex);
        GeoLayoutJumpTable[GL_U8(0x00)]();
    }

    if (getenv("PORT_TRACE_GEO")) Port_DumpGraph(gCurRootGraphNode, 0);
    return gCurRootGraphNode;
}
