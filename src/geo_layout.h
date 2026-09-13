#ifndef _18480_H_
#define _18480_H_

#include "global.h"
#include "src/memory.h"
#include "src/geo_node.h"

#define GEO_CMD_FLAGS_RESET 0
#define GEO_CMD_FLAGS_SET   1
#define GEO_CMD_FLAGS_CLEAR 2

#define CMD_SIZE_SHIFT (sizeof(void *) >> 3)
#define CMD_PROCESS_OFFSET(offset) (((offset) & 3) | (((offset) & ~3) << CMD_SIZE_SHIFT))

#define cur_geo_cmd_u8(offset) \
    (gGeoLayoutCommand[CMD_PROCESS_OFFSET(offset)])

#define cur_geo_cmd_s16(offset) \
    (*(s16 *) &gGeoLayoutCommand[CMD_PROCESS_OFFSET(offset)])

#define cur_geo_cmd_s32(offset) \
    (*(s32 *) &gGeoLayoutCommand[CMD_PROCESS_OFFSET(offset)])

#define cur_geo_cmd_u32(offset) \
    (*(u32 *) &gGeoLayoutCommand[CMD_PROCESS_OFFSET(offset)])

#define cur_geo_cmd_ptr(offset) \
    (*(void **) &gGeoLayoutCommand[CMD_PROCESS_OFFSET(offset)])

typedef void (*GeoLayoutCommandProc)(void);

typedef struct unk_D_800ABE00_cmd0 {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ u16 unk_02; // never read by geo_layout_cmd_branch_and_link; likely a leftover sm64-format field
    /* 0x04 */ u8* branchTarget;
} unk_D_800ABE00_cmd0; // size = 0x8

typedef struct unk_D_800ABE00_cmd1 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd1; // size = 0x4

typedef struct unk_D_800ABE00_cmd2 {
    /* 0x00 */ u8 cmd;
    /* 0x04 */ u8* jumpTarget;
} unk_D_800ABE00_cmd2; // size = 0x8

typedef struct unk_D_800ABE00_cmd3 {
    /* 0x00 */ u8 cmd;
    /* 0x04 */ u8* branchTarget;
} unk_D_800ABE00_cmd3; // size = 0x8

typedef struct unk_D_800ABE00_cmd4 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd4; // size = 0x4

typedef struct unk_D_800ABE00_cmd5 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd5; // size = 0x4

typedef struct unk_D_800ABE00_cmd6 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd6; // size = 0x4

typedef struct unk_D_800ABE00_cmd7 {
    /* 0x00 */ u8 cmd;
    /* 0x04 */ unk_D_8690A610* attachNode; // geo_layout_cmd_attach_node: registered as the current graph node
} unk_D_800ABE00_cmd7; // size = 0x8

typedef struct unk_D_800ABE00_cmd8 {
    /* 0x00 */ u8 cmd;
    /* 0x04 */ func_D_86002F34_000_010 callback; // GeoNode_SetCallback's arg1
    /* 0x08 */ unk_D_86002F34_000_014* callbackData; // GeoNode_SetCallback's arg2
} unk_D_800ABE00_cmd8; // size = 0xC

typedef struct unk_D_800ABE00_cmd9 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd9; // size = 0x4

typedef struct unk_D_800ABE00_cmdA {
    /* 0x00 */ u8 cmd;
    /* 0x04 */ GraphNode* refNode; // GeoNode_CreateWithReference's arg1
} unk_D_800ABE00_cmdA; // size = 0x8

typedef struct unk_D_800ABE00_cmdB {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ u16 fovy;
    /* 0x04 */ u16 viewportX;
    /* 0x06 */ u16 viewportY;
    /* 0x08 */ s16 viewportWidth;
    /* 0x0A */ s16 viewportHeight;
    /* 0x0C */ Vec3s lookAt;
    /* 0x12 */ s16 yaw;
    /* 0x14 */ s16 pitch;
    /* 0x16 */ s16 eyeDistance;
} unk_D_800ABE00_cmdB; // size = 0x18

typedef struct unk_D_800ABE00_cmdC {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 unk_02;
} unk_D_800ABE00_cmdC; // size = 0x4

typedef struct unk_D_800ABE00_cmdD {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 unk_02;
} unk_D_800ABE00_cmdD; // size = 0x4

typedef struct unk_D_800ABE00_cmdE {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 unk_02;
} unk_D_800ABE00_cmdE; // size = 0x4

typedef struct unk_D_800ABE00_cmdF {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 unk_02;
} unk_D_800ABE00_cmdF; // size = 0x4

typedef struct unk_D_800ABE00_cmd10 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 r;
    /* 0x02 */ u8 g;
    /* 0x03 */ u8 b;
} unk_D_800ABE00_cmd10; // size = 0x4

typedef struct unk_D_800ABE00_cmd11 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd11; // size = 0x4

typedef struct unk_D_800ABE00_cmd12 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd12; // size = 0x4

typedef struct unk_D_800ABE00_cmd13 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 r;
    /* 0x02 */ u8 g;
    /* 0x03 */ u8 b;
    /* 0x04 */ s16 fogNear;
    /* 0x06 */ s16 fogFar;
} unk_D_800ABE00_cmd13; // size = 0x8

typedef struct unk_D_800ABE00_cmd14 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[0x3];
    /* 0x04 */ s16 pitch; // GeoNode_CreateLight's arg2, scaled (val<<0xF)/180
    /* 0x06 */ s16 yaw; // GeoNode_CreateLight's arg3, scaled (val<<0xF)/180
    /* 0x08 */ u8 r;
    /* 0x09 */ u8 g;
    /* 0x0A */ u8 b;
    /* 0x0B */ u8 a;
} unk_D_800ABE00_cmd14; // size = 0xC

typedef struct unk_D_800ABE00_cmd15 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[0xB];
} unk_D_800ABE00_cmd15; // size = 0xC

typedef struct unk_D_800ABE00_cmd16 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 r;
    /* 0x02 */ u8 g;
    /* 0x03 */ u8 b;
} unk_D_800ABE00_cmd16; // size = 0x4

typedef struct unk_D_800ABE00_cmd17 {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 dataCount1; // GeoNode_CreateShadowContext's arg2; paired with data1
    /* 0x04 */ s16 dataCount2; // arg4; paired with data2
    /* 0x06 */ s16 vtxCount; // arg6; paired with vtxData
    /* 0x08 */ unk_D_86002F34_018* data1; // arg3
    /* 0x0C */ unk_D_86002F34_01C* data2; // arg5
    /* 0x10 */ Vtx* vtxData; // arg7
} unk_D_800ABE00_cmd17; // size = 0x14

typedef struct unk_D_800ABE00_cmd18 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} unk_D_800ABE00_cmd18; // size = 0x8

typedef struct unk_D_800ABE00_cmd19 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[0x3];
    /* 0x04 */ s16 nearDistance; // Geo_NodeCullDistance: children render only when nearDistance <= depth < farDistance
    /* 0x06 */ s16 farDistance;
} unk_D_800ABE00_cmd19; // size = 0x8

typedef struct unk_D_800ABE00_cmd1A {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 caseIndex; // Geo_NodeSwitchCase: walks this many siblings from the first child before dispatching
} unk_D_800ABE00_cmd1A; // size = 0x4

typedef struct unk_D_800ABE00_cmd1B {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[0x3];
    /* 0x04 */ s16 rotX; // degrees, scaled (val<<0xF)/180
    /* 0x06 */ s16 rotY;
    /* 0x08 */ s16 rotZ;
    /* 0x0A */ Vec3s translation;
} unk_D_800ABE00_cmd1B; // size = 0x10

typedef struct unk_D_800ABE00_cmd1C {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[0x3];
    /* 0x04 */ u32 translateX; // Q16 fixed point, /65536.0f
    /* 0x08 */ u32 translateY;
    /* 0x0C */ u32 translateZ;
} unk_D_800ABE00_cmd1C; // size = 0x10

typedef struct unk_D_800ABE00_cmd1D {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 partIndex; // GeoNode_CreateAnimatedPart's arg2
    /* 0x02 */ u8 flags; // bit0/bit1 remapped into GeoNode_CreateAnimatedPart's arg3
    /* 0x03 */ u8 animIndex; // GeoNode_CreateAnimatedPart's arg4
    /* 0x04 */ Vec3s rotationA; // GeoNode_CreateAnimatedPart's arg5
    /* 0x0A */ Vec3s rotationB; // GeoNode_CreateAnimatedPart's arg6
    /* 0x10 */ u32 translateX; // Q16 fixed point, /65536.0f
    /* 0x14 */ u32 translateY;
    /* 0x18 */ u32 translateZ;
} unk_D_800ABE00_cmd1D; // size = 0x1C

typedef struct unk_D_800ABE00_cmd1E {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 partIndex; // GeoNode_CreateDisplayListPart's arg2
    /* 0x02 */ s16 unk_02; // GeoNode_CreateDisplayListPart's arg4
    /* 0x04 */ Gfx* displayList;
} unk_D_800ABE00_cmd1E; // size = 0x8

typedef struct unk_D_800ABE00_cmd1F {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 modelIndex; // GeoNode_CreateModelPart's arg2
    /* 0x04 */ s16 rotX; // degrees, scaled (val<<0xF)/180
    /* 0x06 */ s16 rotY;
    /* 0x08 */ s16 rotZ;
    /* 0x0A */ Vec3s position;
    /* 0x10 */ s16 scaleX; // /100.0f
    /* 0x12 */ s16 scaleY;
    /* 0x14 */ s16 scaleZ;
    /* 0x16 */ char pad[0x2];
} unk_D_800ABE00_cmd1F; // size = 0x18

typedef struct unk_D_800ABE00_cmd20 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 partIndex; // GeoNode_CreateDisplayListMatrixFromTransform's arg2
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 rotX; // degrees, scaled (val<<0xF)/180
    /* 0x06 */ s16 rotY;
    /* 0x08 */ s16 rotZ;
    /* 0x0A */ Vec3s position;
    /* 0x10 */ Gfx* displayList;
} unk_D_800ABE00_cmd20; // size = 0x14

typedef struct unk_D_800ABE00_cmd21 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 partIndex; // GeoNode_CreateScale's arg2
    /* 0x02 */ Vec3s position;
    /* 0x08 */ u32 scale; // Q16 fixed point, /65536.0f
    /* 0x0C */ Gfx* displayList;
} unk_D_800ABE00_cmd21; // size = 0x10

typedef struct unk_D_800ABE00_cmd22 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 partIndex; // GeoNode_CreateDisplayList's arg2
    /* 0x04 */ Gfx* displayList;
} unk_D_800ABE00_cmd22; // size = 0x8

typedef struct unk_D_800ABE00_cmd23 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 colorOverrideFlag; // Geo_NodeShadowTexture: ==1 forces the shadow color to opaque white
    /* 0x02 */ s16 eventFrame; // ModelAnim_GetEventAtFrame's frame arg
    /* 0x04 */ Gfx* displayList;
    /* 0x07 */ s16 textureIndex1; // indexes D_8006F0A0's first texture array when >= 0
    /* 0x0A */ s16 textureIndex2; // indexes D_8006F0A0's second texture array when >= 0
    /* 0x0C */ u8 r;
    /* 0x0D */ u8 g;
    /* 0x0E */ u8 b;
    /* 0x0F */ u8 a;
} unk_D_800ABE00_cmd23; // size = 0x10

typedef struct unk_D_800ABE00_cmd24 {
    /* 0x00 */ u8 cmd;
    /* 0x02 */ s16 anchorId; // GeoNode_CreateAnchor's arg2
} unk_D_800ABE00_cmd24; // size = 0x4

typedef struct unk_D_800ABE00_cmd25 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ char pad[3];
} unk_D_800ABE00_cmd25; // size = 0x4

typedef struct unk_D_800ABE00_cmd26 {
    /* 0x00 */ u8 cmd;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ s8 unk_03;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ u8 r;
    /* 0x11 */ u8 g;
    /* 0x12 */ u8 b;
    /* 0x13 */ u8 a;
} unk_D_800ABE00_cmd26; // size = 0x14

void geo_layout_cmd_branch_and_link(void); // cmd  0
void geo_layout_cmd_end(void); // cmd  1
void geo_layout_cmd_jump(void); // cmd  2
void geo_layout_cmd_branch(void); // cmd  3
void geo_layout_cmd_return(void); // cmd  4
void geo_layout_cmd_open_node(void); // cmd  5
void geo_layout_cmd_close_node(void); // cmd  6
void geo_layout_cmd_attach_node(void); // cmd  7
void geo_layout_cmd_set_callback(void); // cmd  8
void geo_layout_cmd_create_container(void); // cmd  9
void geo_layout_cmd_create_reference(void); // cmd 10
void geo_layout_cmd_create_camera(void); // cmd 11
void geo_layout_cmd_create_type3(void); // cmd 12
void geo_layout_cmd_create_type4(void); // cmd 13
void geo_layout_cmd_create_ortho(void); // cmd 14
void geo_layout_cmd_create_perspective(void); // cmd 15
void geo_layout_cmd_create_background(void); // cmd 16
void geo_layout_cmd_create_clear_depth(void); // cmd 17
void geo_layout_cmd_noop(void); // cmd 18
void geo_layout_cmd_create_fog(void); // cmd 19
void geo_layout_cmd_create_light(void); // cmd 20
void geo_layout_cmd_skip(void); // cmd 21
void geo_layout_cmd_create_ambient_light(void); // cmd 22
void geo_layout_cmd_create_shadow_context(void); // cmd 23
void geo_layout_cmd_create_shadow(void); // cmd 24
void geo_layout_cmd_create_cull_distance(void); // cmd 25
void geo_layout_cmd_create_switch_case(void); // cmd 26
void geo_layout_cmd_create_translate_rotate(void); // cmd 27
void geo_layout_cmd_create_translate(void); // cmd 28
void geo_layout_cmd_create_animated_part(void); // cmd 29
void geo_layout_cmd_create_display_list_part(void); // cmd 30
void geo_layout_cmd_create_model_part(void); // cmd 31
void geo_layout_cmd_create_display_list_matrix(void); // cmd 32
void geo_layout_cmd_create_scale(void); // cmd 33
void geo_layout_cmd_create_display_list(void); // cmd 34
void geo_layout_cmd_create_shadow_texture(void); // cmd 35
void geo_layout_cmd_create_anchor(void); // cmd 36
void geo_layout_cmd_set_node_flag4(void); // cmd 37
void geo_layout_cmd_create_group(void); // cmd 38
GraphNode* process_geo_layout(MemoryBlock*, void*);

#endif // _18480_H_
