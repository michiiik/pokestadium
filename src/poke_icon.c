#include "poke_icon.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/jpeg_decoder.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/dp_intro.h"
#include "src/geo_layout.h"
#include "src/memory.h"

static unk_D_86002F58_004_000* D_800AC830;
static s32 D_800AC834;

s32 PokeIcon_CapturePreviewGeoNode(s32, GraphNode*);
s32 PokeIcon_CaptureGeoNode(s32, GraphNode*);

s32 PokeIcon_CaptureGeoNode(s32 arg0, GraphNode* arg1) {
    if (arg0 == 0) {
        D_800ABE10.archives.capturedSceneNode = arg1;
    }
    return 0;
}

void PokeIcon_InitRenderScene(unk_func_8001A024* arg0, s16 arg1, s16 arg2) {
    static u32 D_8006F350[] = {
        0x0C00FFFF,    0x05000000, 0x0B00001E, 0x00000000, 0x00300030, 0x00000000, 0x0000000A, 0x015404E2, 0x05000000,
        0x0D000000,    0x05000000, 0x14000000, 0x002D0019, 0xFFFFFF32, 0x14000000, 0xFFD300CD, 0x80808032, 0x16646464,
        0x0F000001,    0x05000000, 0x1F000000, 0x00000000, 0x00000000, 0xFF060000, 0x00640064, 0x00640000, 0x08000000,
        PokeIcon_CaptureGeoNode, 0x00000000, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000, 0x00000000, 0x00000000,
    };

    unk_D_86002F34_00C* sp24;
    MemoryBlock* sp20 = MainPool_AllocState(main_pool_get_available(), 0);

    arg0->sceneRoot = process_geo_layout(sp20, D_8006F350);
    arg0->displayObject = D_800ABE10.archives.capturedSceneNode;
    MainPool_FinalizeAllocation(sp20);
    sp24 = arg0->sceneRoot->unk_0C;
    GeoCamera_SetViewport(sp24, 0, 0, arg1, arg2);
    GeoCamera_SetPerspective(sp24, 30.0f, 100.0f, 12800.0f);
}

unk_func_8001A024* PokeIcon_CreateRenderList(unk_D_86002F58_004_000_010* arg0, s16 arg1, s16 arg2, s16 arg3) {
    s32 var_s0;
    unk_D_80068BB0* temp_a1;
    s32 i;
    unk_func_8001A024* temp_v0 = main_pool_alloc(sizeof(unk_func_8001A024), 0);

    if (temp_v0 != NULL) {
        temp_v0->flags = 0;
        temp_v0->loadCursor = 0;
        temp_v0->lastRenderedSlot = -1;
        temp_v0->slotCount = arg1;
        temp_v0->slots = main_pool_alloc(arg1 * sizeof(unk_func_8001A024_008), 0);
        temp_v0->loadContext = arg0;
        temp_v0->depthBuffer = NULL;
        temp_v0->nameTable = D_8006FF00;

        if (arg0 != NULL) {
            temp_v0->depthBuffer = GfxImage_Allocate(0, 2, arg2, arg3, 1);
        }

        for (i = 0; i < arg1; i++) {
            temp_v0->slots[i].flags = 0;
            temp_v0->slots[i].speciesRef.raw = 0;
            temp_v0->slots[i].image = GfxImage_Allocate(0, 2, arg2, arg3, 0);
            bzero(temp_v0->slots[i].image->img_p, arg2 * arg3 * 2);
            if (temp_v0->depthBuffer != NULL) {
                GfxImage_AttachDepthBuffer(temp_v0->slots[i].image, temp_v0->depthBuffer);
            }
        }

        PokeIcon_InitRenderScene(temp_v0, arg2, arg3);
    }
    return temp_v0;
}

void PokeIcon_AttachDepthBuffer(unk_func_8001A024* arg0, unk_D_86002F58_004_000_010* arg1, unk_D_80068BB0* arg2) {
    s32 i;

    arg0->loadContext = arg1;
    arg0->depthBuffer = arg2;

    for (i = 0; i < arg0->slotCount; ++i) {
        GfxImage_AttachDepthBuffer(arg0->slots[i].image, arg2);
    }
}

void PokeIcon_SetNameTable(unk_func_8001A024* arg0, unk_D_8006FF00* arg1) {
    arg0->nameTable = arg1;
}

void PokeIcon_ResetRenderList(unk_func_8001A024* arg0) {
    // s32 var_s0;
    s32 i;
    u16 w;
    u16 h;
    unk_func_8001A024_008* tempImg_p;
    unk_D_80068BB0* tempDepth_p;

    if (arg0->flags & 1) {
        PokeIcon_WaitFrameLoad(arg0->loadContext);
        ModelRenderer_ClearDisplayObject(arg0->displayObject);
    }

    arg0->flags = 0;
    arg0->loadCursor = 0;
    arg0->lastRenderedSlot = -1;

    for (i = 0; i < arg0->slotCount; ++i) {
        tempImg_p = &arg0->slots[i];
        tempDepth_p = tempImg_p->image;
        w = tempDepth_p->width;
        h = tempDepth_p->height;
        tempImg_p->flags = 0;
        arg0->slots[i].speciesRef.raw = 0;
        bzero(arg0->slots[i].image->img_p, w * h * 2);
    }
}

#ifdef NON_MATCHING
void PokeIcon_SetSlotFromSpecies(unk_func_8001A024* arg0, s32 arg1, s32 arg2, u16 arg3) {
    s32 w;
    s32 h;

    if (arg1 >= arg0->slotCount) {
        return;
    }

    w = arg0->slots[arg1].image->width;
    h = arg0->slots[arg1].image->height;
    bzero(arg0->slots[arg1].image->img_p, w * h * 2);

    if ((arg2 <= 0) || (arg2 >= 0x98)) {
        arg2 = 0x98;
    }

    arg0->slots[arg1].flags = 1;
    arg0->slots[arg1].paletteMask = arg3 & ~1;
    arg0->slots[arg1].speciesRef.raw = arg2;
    arg0->slots[arg1].scale = arg0->nameTable[arg2 - 1].unk_02 / 100.0f;
    arg0->slots[arg1].sizeVariant.raw = 0;

    if (arg0->nameTable[arg2 - 1].name == NULL) {
        arg0->slots[arg1].cameraAngle = arg0->nameTable[arg2 - 1].unk_08 - 0xE38;
    } else {
        arg0->slots[arg1].cameraAngle = -0xE38;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/poke_icon/PokeIcon_SetSlotFromSpecies.s")
#endif

#ifdef NON_MATCHING
void PokeIcon_SetSlotFromMon(unk_func_8001A024* arg0, s32 arg1, BattleMon* arg2, u16 arg3) {
    s32 w;
    s32 h;

    if (arg1 >= arg0->slotCount) {
        return;
    }
    w = arg0->slots[arg1].image->width;
    h = arg0->slots[arg1].image->height;
    bzero(arg0->slots[arg1].image->img_p, w * h * 2);
    if (((s32) arg2->species.dexId > 0) && ((s32) arg2->species.dexId < 0x98)) {
        arg0->slots[arg1].flags = 3;
        arg0->slots[arg1].speciesRef.raw = arg2;;
        arg0->slots[arg1].paletteMask = arg3 & ~1;
        arg0->slots[arg1].scale = arg0->nameTable[arg2->species.dexId - 1].unk_02 / 100.0f;
        Model_ComputeSizeVariant(&arg0->slots[arg1].sizeVariant, arg2);
        if (arg0->nameTable[arg2->species.dexId - 1].name == NULL) {
            arg0->slots[arg1].cameraAngle = arg0->nameTable[arg2->species.dexId - 1].unk_08 - 0xE38;;
            return;
        }
        arg0->slots[arg1].cameraAngle  = -0xE38;
        return;
    }
    arg0->slots[arg1].flags = 1;
    arg0->slots[arg1].paletteMask = arg3 & ~1;
    arg0->slots[arg1].speciesRef.raw = 0x98;
    arg0->slots[arg1].scale = (f32) ((f32) arg0->nameTable[151].unk_02 / 100.0f);
    arg0->slots[arg1].sizeVariant.raw = 0;
    arg0->slots[arg1].cameraAngle = -0xE38;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/poke_icon/PokeIcon_SetSlotFromMon.s")
#endif

void PokeIcon_AdjustImageColors(u16* arg0, s32 a1, arg1_func_80010CA8 arg2) {
    s32 cond;
    s32 arg1 = a1;

    if (!arg0) {}

    arg1 *= 2;
    osInvalDCache(arg0, arg1);

    cond = arg1 >= 2;
    arg1 -= 2;
    // clang-format off
    while (cond != 0) {        
        *arg0 = Color_AdjustRGB5551(*arg0, arg2);        cond = arg1 >= 2;        arg1 -= 2;
        arg0++;    
    }
    // clang-format on
}

void PokeIcon_AdjustImageColorBuffer(unk_D_80068BB0* arg0, arg1_func_80010CA8 arg1) {
    s32 size = arg0->width * arg0->height;

    PokeIcon_AdjustImageColors(arg0->img_p, size, arg1);
}

void PokeIcon_RenderSlot(unk_func_8001A024* arg0, s32 arg1) {
    Vec3s sp38;
    unk_D_86002F34_00C* sp34;
    unk_func_8001A024_008* temp_s1;
    unk_D_86002F58_004_000_004* sp2C;

    sp34 = arg0->sceneRoot->unk_0C;
    temp_s1 = &arg0->slots[arg1];
    sp2C = arg0->loadContext->lastLoadedFragment->unk_08->unk_00[0];

    ModelRenderer_ClearDisplayObject(arg0->displayObject);
    ModelAnim_ClearTransformChannel(arg0->displayObject);

    if (temp_s1->flags & 2) {
        sp38.x = (s16)(arg0->nameTable[*temp_s1->speciesRef.monDexIdPtr - 1].unk_14 >> 0x10) >> 6;
        sp38.y = (s16)(arg0->nameTable[*temp_s1->speciesRef.monDexIdPtr - 1].unk_14 >> 6) >> 4;
        sp38.z = (s16)(arg0->nameTable[*temp_s1->speciesRef.monDexIdPtr - 1].unk_14 << 6) >> 6;
        Model_InitDisplayObject(arg0->displayObject, 0, *temp_s1->speciesRef.monDexIdPtr, sp2C);
        ModelAnim_SetAnimation(arg0->displayObject, 1);
        ModelAnim_SetFrame(arg0->displayObject, arg0->nameTable[*temp_s1->speciesRef.monDexIdPtr - 1].unk_01);
    } else {
        sp38.x = (s16)(arg0->nameTable[temp_s1->speciesRef.raw - 1].unk_14 >> 0x10) >> 6;
        sp38.y = (s16)(arg0->nameTable[temp_s1->speciesRef.raw - 1].unk_14 >> 6) >> 4;
        sp38.z = (s16)(arg0->nameTable[temp_s1->speciesRef.raw - 1].unk_14 << 6) >> 6;
        Model_InitDisplayObject(arg0->displayObject, 0, temp_s1->speciesRef.dexId, sp2C);
        ModelAnim_SetAnimation(arg0->displayObject, 1);
        ModelAnim_SetFrame(arg0->displayObject, arg0->nameTable[temp_s1->speciesRef.raw - 1].unk_01);
    }
    Vec3f_FromVec3s(&arg0->displayObject->unk_024, &sp38);
    arg0->displayObject->unk_000.unk_02 &= ~0x40;
    arg0->displayObject->unk_000.unk_02 |= 0x80;
    Vec3f_SetComponentsDuplicate(&arg0->displayObject->unk_030, temp_s1->scale, temp_s1->scale, temp_s1->scale);
    Camera_ComputeEyeFromAngles(&sp34->unk_60.at, &sp34->unk_60.eye, 1250.0f, 0x71C, temp_s1->cameraAngle);
    GfxImage_SetRenderTarget(&gDisplayListHead, arg0->slots[arg1].image);
    GeoRender_AdvanceFrameCounter();
    GeoRender_SetMode(3);
    Geo_RenderRootNode(arg0->sceneRoot);
    GfxImage_FillCurrent(&gDisplayListHead, arg0->slots[arg1].paletteMask);
    GeoRender_SetMode(1);
    Geo_RenderRootNode(arg0->sceneRoot);
}

s32 PokeIcon_ProcessNextSlot(unk_func_8001A024* arg0, s32 arg1) {
    s32 i;
    s32 var_v0;
    s32 sp24;
    s32 sp20;
    unk_func_8001A024_008* temp_v0_2;

    sp24 = -1;
    sp20 = arg0->loadCursor;
    if (arg0->flags & 1) {
        if (arg1 == 1) {
            var_v0 = PokeIcon_WaitFrameLoad(arg0->loadContext);
        } else {
            var_v0 = PokeIcon_PollFrameLoad(arg0->loadContext);
        }
        if (var_v0 != 0) {
            arg0->flags &= 0xFFFE;
            arg0->slots[sp20].flags &= 0xFFFE;
            PokeIcon_RenderSlot(arg0, sp20);
            arg0->lastRenderedSlot = sp20;
            sp24 = sp20;
            arg0->loadCursor++;
            if (arg0->loadCursor >= arg0->slotCount) {
                arg0->loadCursor = 0;
            }
        }
    } else {
        for (i = 0; i < arg0->slotCount; i++) {
            temp_v0_2 = &arg0->slots[sp20];
            if (temp_v0_2->flags & 1) {
                arg0->flags |= 1;
                if (temp_v0_2->flags & 2) {
                    PokeIcon_RequestFrameLoad(arg0->loadContext, *temp_v0_2->speciesRef.monDexIdPtr, temp_v0_2->sizeVariant);
                } else {
                    PokeIcon_RequestFrameLoad(arg0->loadContext, temp_v0_2->speciesRef.dexId, temp_v0_2->sizeVariant);
                }
                arg0->loadCursor = sp20;
                break;
            } else {
                sp20 += 1;
                if (sp20 >= arg0->slotCount) {
                    sp20 = 0;
                }
            }
        }
    }
    return sp24;
}

s32 PokeIcon_ProcessAllSlots(unk_func_8001A024* arg0, s32 arg1) {
    UNUSED s32 pad;
    arg1_func_80010CA8 sp48;
    s32 sp44;
    s32 var_s1;
    unk_func_8001A024_008* temp_s0;
    unk_func_8001A024_008* temp_s1;
    s32 i;

    sp44 = -1;
    var_s1 = arg0->loadCursor;
    if (arg0->lastRenderedSlot >= 0) {
        temp_s0 = &arg0->slots[arg0->lastRenderedSlot];
        if (temp_s0->flags & 4) {
            while (Display_IsFrameReady() == 0) {}
            PokeIcon_AdjustImageColorBuffer(temp_s0->image, temp_s0->sizeVariant);
            temp_s0->flags &= ~4;
        }
    }

    for (i = 0; i < arg0->slotCount; i++) {
        temp_s1 = &arg0->slots[var_s1];
        if (temp_s1->flags & 1) {
            if (Display_IsFrameReady() == 0) {
                do { } while (Display_IsFrameReady() == 0); }

            sp48.raw = 0;
            if (temp_s1->flags & 2) {
                PokeIcon_RequestFrameLoad(arg0->loadContext, *temp_s1->speciesRef.monDexIdPtr, sp48);
                if (temp_s1->sizeVariant.raw != 0) {
                    temp_s1->flags |= 4;
                }
            } else {
                PokeIcon_RequestFrameLoad(arg0->loadContext, temp_s1->speciesRef.dexId, sp48);
            }

            PokeIcon_WaitFrameLoad(arg0->loadContext);
            arg0->slots[var_s1].flags &= ~1;
            PokeIcon_RenderSlot(arg0, var_s1);
            sp44 = var_s1;
            arg0->loadCursor = var_s1;
            arg0->lastRenderedSlot = var_s1;
            arg0->loadCursor++;
            if (arg0->loadCursor >= arg0->slotCount) {
                arg0->loadCursor = 0;
            }
            break;
        }

        var_s1 += 1;
        if (var_s1 >= arg0->slotCount) {
            var_s1 = 0;
        }
    }

    return sp44;
}

s32 PokeIcon_RenderNextSlotBlocking(unk_func_8001A024* arg0, s32 arg1) {
    s32 i;
    s32 var_a2;
    s32 var_s0;
    unk_func_8001A024_008* temp_a0;
    unk_func_8001A024_008* temp_s1;
    
    var_a2 = -1;
    var_s0 = arg0->loadCursor;
    temp_a0 = arg0->slots;
    
    for (i = 0; i < arg0->slotCount; i++) {
        temp_s1 = &arg0->slots[var_s0];
        if (temp_s1->flags & 1) {
            while (Display_IsFrameReady() == 0) {}
            
            if (temp_s1->flags & 2) {
                PokeIcon_RequestFrameLoad(arg0->loadContext, *temp_s1->speciesRef.monDexIdPtr, temp_s1->sizeVariant);
            } else {
                PokeIcon_RequestFrameLoad(arg0->loadContext, temp_s1->speciesRef.dexId, temp_s1->sizeVariant);
            }
            PokeIcon_WaitFrameLoad(arg0->loadContext);
            arg0->slots[var_s0].flags &= ~1;
            PokeIcon_RenderSlot(arg0, var_s0);
            arg0->loadCursor = var_s0;
            var_a2 = var_s0;
            arg0->loadCursor++;
            arg0->lastRenderedSlot = var_s0;
            if (arg0->loadCursor >= arg0->slotCount) {
                arg0->loadCursor = 0;
            }
            break;
        } else {
            var_s0 += 1;
            if (var_s0 >= arg0->slotCount) {
                var_s0 = 0;
            }
        }
    }
    return var_a2;
}

void PokeIcon_RedrawSlot(unk_func_8001A024* arg0, s32 arg1) {
    s32 i;

    if (arg1 == arg0->lastRenderedSlot) {
        GfxImage_SetRenderTarget(&gDisplayListHead, arg0->slots[arg1].image);
        GfxImage_FillCurrent(&gDisplayListHead, arg0->slots[arg1].paletteMask);
        GeoRender_SetMode(1);
        Geo_RenderRootNode(arg0->sceneRoot);
        return;
    }
    for (i = arg0->slotCount; i > 0; i--) {
        if (arg0->slots[i - 1].flags & 1) {
            break;
        }
    }
    if (i == 0) {
        arg0->slots[arg1].flags |= 1;
    }
}

u8* PokeIcon_GetImage(unk_func_8001A024* arg0, s32 arg1) {
    u8* image;
    image = NULL;
    if (arg1 < (s32)arg0->slotCount) {
        image = arg0->slots[arg1].image->img_p;
    }
    return image;
}

s32 PokeIcon_AreSlotsIdle(unk_func_8001A024* arg0) {
    s32 i;
    
    for (i = 0; i < arg0->slotCount; i++) {
        if (arg0->slots[i].flags & 5) {
            return 0;
        }
    }
    return 1;
}

void PokeIcon_OpenModelArchive(void) {
    D_800ABE10.archives.textureArchive = BinArchive_Open(0x820000, NULL, 1, 1);
}

void PokeIcon_LoadModelTexture(u16* arg0, s32 arg1) {
    BinArchive* temp_v1;
    u8* temp_a1;

    if ((arg1 > 0) && (arg1 < 0x99)) {
        temp_v1 = D_800ABE10.archives.textureArchive;
        temp_a1 = temp_v1->romStart + temp_v1[arg1].raw;
        Dma_WriteChunks(arg0, temp_a1, temp_v1[arg1].romStart + temp_a1, 0);
    }
}

void PokeIcon_LoadModelTextureForMon(u16* arg0, s32 arg1, BattleMon* arg2) {
    arg1_func_80010CA8 sp1C;

    if (arg2 == NULL) {
        PokeIcon_LoadModelTexture(arg0, arg1);
        return;
    }

    Model_ComputeSizeVariant(&sp1C, arg2);
    PokeIcon_LoadModelTexture(arg0, arg2->species.dexId);
    PokeIcon_AdjustImageColors(arg0, 0x640, sp1C);
}

s32 PokeIcon_CapturePreviewGeoNode(s32 arg0, GraphNode* arg1) {
    if (arg0 == 0) {
        D_800AC830 = arg1;
    }
    return 0;
}

void PokeIcon_InitPreviewScene(unk_func_8001B1FC* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    static u32 D_8006F3E0[] = {
        0x0C00FFFF,    0x05000000, 0x0B00001E, 0x00000000, 0x010000E0, 0x00000000, 0x0000000A, 0x015404E2, 0x05000000,
        0x0D000000,    0x05000000, 0x14000000, 0x002D013B, 0xFFFFFF32, 0x14000000, 0xFFD30087, 0x80808032, 0x16646464,
        0x0F000001,    0x05000000, 0x1F000000, 0x00000000, 0x00000000, 0xFF060000, 0x00640064, 0x00640000, 0x08000000,
        PokeIcon_CapturePreviewGeoNode, 0x00000000, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000, 0x00000000, 0x00000000,
    };
    MemoryBlock* sp24;
    MemoryBlock* temp_v0;

    sp24 = MainPool_AllocState(main_pool_get_available(), 0);
    arg0->sceneRoot = process_geo_layout(sp24, D_8006F3E0);
    arg0->displayObject = D_800AC830;
    MainPool_FinalizeAllocation(sp24);
    GeoCamera_SetViewport(arg0->sceneRoot->unk_0C, arg1, arg2, arg3, arg4);
    GeoCamera_SetPerspective(arg0->sceneRoot->unk_0C, 30.0f, 100.0f, 12800.0f);
}

unk_func_8001B1FC* PokeIcon_CreateModelPreview(unk_D_86002F58_004_000_010* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5,
                                 s16 arg6, u8* arg7) {
    unk_func_8001B1FC* temp_v0 = main_pool_alloc(sizeof(unk_func_8001B1FC), 0);

    if (temp_v0 != NULL) {
        temp_v0->flags = 6;
        temp_v0->loadContext = arg0;
        temp_v0->unk_14 = 0;
        temp_v0->imageWidth = arg1;
        temp_v0->imageHeight = arg2;
        temp_v0->tileWidth = arg5;
        temp_v0->tileHeight = arg6;
        temp_v0->unk_0C = arg7;
        temp_v0->image = GfxImage_Allocate(0, 2, arg1, arg2, 0);
        temp_v0->depthBuffer = GfxImage_Allocate(0, 2, arg1, arg2, 1);
        GfxImage_AttachDepthBuffer(temp_v0->image, temp_v0->depthBuffer);
        PokeIcon_InitPreviewScene(temp_v0, arg3, arg4, arg5, arg6);
    }
    return temp_v0;
}

void PokeIcon_SetPreviewMon(unk_func_8001B1FC* arg0, BattleMon* arg1, u16 arg2) {
    arg1_func_80010CA8 sp2C;
    s16 tmp1 = (s16)(D_8006FF00[arg1->species.dexId - 1].unk_14 >> 0x10) >> 6;
    s16 tmp2 = (s16)(D_8006FF00[arg1->species.dexId - 1].unk_14 >> 6) >> 4;
    s16 tmp3 = (s16)(D_8006FF00[arg1->species.dexId - 1].unk_14 << 6) >> 6;

    arg0->flags |= 1;
    arg0->unk_14 = arg1;
    arg0->rotateSpeed = 0x100;
    arg0->idleTimer = 0;
    arg0->paletteMask = arg2;
    arg0->modelScale = D_8006FF00[arg1->species.dexId - 1].unk_02 / 100.0f;

    arg0->displayObject->unk_024.x = tmp1;
    arg0->displayObject->unk_024.y = tmp2;
    arg0->displayObject->unk_024.z = tmp3;

    arg0->displayObject->unk_01E = D_8006F05C;
    Model_ComputeSizeVariant(&sp2C, arg1);
    PokeIcon_RequestFrameLoad(arg0->loadContext, arg1->species.dexId, sp2C);
    PokeIcon_WaitFrameLoad(arg0->loadContext);
    ModelRenderer_ClearDisplayObject(arg0->displayObject);
    Model_InitDisplayObject(arg0->displayObject, 0, arg1->species.dexId, arg0->loadContext->lastLoadedFragment->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(arg0->displayObject, 1);
    arg0->displayObject->unk_000.unk_02 &= ~0x40;
    arg0->displayObject->unk_000.unk_02 |= 0x80;
}

void PokeIcon_SetPreviewSpecies(unk_func_8001B1FC* arg0, s32 arg1, u16 arg2) {
    arg1_func_80010CA8 sp34;
    s16 tmp1 = (s16)(D_8006FF00[arg1 - 1].unk_14 >> 0x10) >> 6;
    s16 tmp2 = (s16)(D_8006FF00[arg1 - 1].unk_14 >> 6) >> 4;
    s16 tmp3 = (s16)(D_8006FF00[arg1 - 1].unk_14 << 6) >> 6;

    arg0->flags &= ~1;
    arg0->unk_14 = arg1;
    arg0->rotateSpeed = 0x100;
    arg0->idleTimer = 0;
    arg0->paletteMask = arg2;
    arg0->modelScale = D_8006FF00[arg1 - 1].unk_02 / 100.0f;

    arg0->displayObject->unk_024.x = tmp1;
    arg0->displayObject->unk_024.y = tmp2;
    arg0->displayObject->unk_024.z = tmp3;

    arg0->displayObject->unk_01E = D_8006F05C;
    sp34.raw = 0;
    PokeIcon_RequestFrameLoad(arg0->loadContext, arg1, sp34);
    PokeIcon_WaitFrameLoad(arg0->loadContext);
    ModelRenderer_ClearDisplayObject(arg0->displayObject);
    Model_InitDisplayObject(arg0->displayObject, 0, arg1, arg0->loadContext->lastLoadedFragment->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(arg0->displayObject, 1);
    arg0->displayObject->unk_000.unk_02 &= ~0x40;
    arg0->displayObject->unk_000.unk_02 |= 0x80;
}

s32 PokeIcon_HandlePreviewInput(Vec3s* arg0) {
    s32 var_v1 = 0;

    if (gPlayer1Controller->buttonDown & 0x800) {
        var_v1 = 1;
        arg0->x += 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x400) {
        var_v1 = 1;
        arg0->x -= 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x200) {
        var_v1 = 1;
        arg0->y += 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x100) {
        var_v1 = 1;
        arg0->y -= 0x400;
    }

    return var_v1;
}

s32 func_8001B6BC(Vec3s* arg0) {
    return 0;
}

void PokeIcon_AdvancePreviewMotion(unk_func_8001B1FC* arg0) {
    s32 var_a0;
    Vec3s* sp18 = &arg0->displayObject->unk_01E;
    s16 var_a2;
    s16 temp_lo;

    if (arg0->rotateSpeed < 0x100) {
        arg0->rotateSpeed += 0x10;
    }
    sp18->y += arg0->rotateSpeed;
    var_a2 = arg0->rotateSpeed;

    if (sp18->x < 0) {
        var_a0 = -sp18->x;
    } else {
        var_a0 = sp18->x;
    }

    temp_lo = var_a0 / 12;
    if (temp_lo < 8) {
        temp_lo = 8;
    }

    if (temp_lo < var_a2) {
        var_a2 = temp_lo;
    }
    sp18->x = Math_StepToS(sp18->x, 0, var_a2);
}

void PokeIcon_UpdatePreview(unk_func_8001B1FC* arg0) {
    f32 temp_fv0;
    s16 temp_v0_2;
    s32 temp_v0;
    s32 var_v1;

    var_v1 = 0;
    if (arg0->flags & 2) {
        var_v1 = PokeIcon_HandlePreviewInput(&arg0->displayObject->unk_01E);
        if (var_v1 == 0) {
            var_v1 = func_8001B6BC(&arg0->displayObject->unk_01E);
        }
    }

    if (var_v1 != 0) {
        arg0->rotateSpeed = 0;
        arg0->idleTimer = 0x78;
    } else if (arg0->flags & 4) {
        if (arg0->idleTimer > 0) {
            arg0->idleTimer--;
        } else {
            PokeIcon_AdvancePreviewMotion(arg0);
        }
    }

    Vec3f_SetComponentsDuplicate(&D_800AC830->unk_030, arg0->modelScale, arg0->modelScale, arg0->modelScale);
}

void PokeIcon_DrawPreview(unk_func_8001B1FC* arg0) {
    s32 var_v0;
    s32 var_s2;
    s32 i;

    if (arg0->unk_0C == 0) {
        GfxImage_FillCurrent(&gDisplayListHead, arg0->paletteMask);
        return;
    }

    var_s2 = arg0->unk_0C;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    for (i = 0; i < arg0->tileHeight; i += 8) {
        var_v0 = arg0->tileHeight - i;
        if (var_v0 >= 9) {
            var_v0 = 8;
        }
        Gfx_DrawTextureRgba16(0, i, arg0->tileWidth, var_v0, var_s2, arg0->imageWidth, 0x200000);
        var_s2 += arg0->imageWidth * 0x10;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);

    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, arg0->tileWidth, arg0->tileHeight);
}

u8* PokeIcon_RenderPreview(unk_func_8001B1FC* arg0) {
    PokeIcon_UpdatePreview(arg0);
    GfxImage_SetRenderTarget(&gDisplayListHead, arg0->image);
    PokeIcon_DrawPreview(arg0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(arg0->sceneRoot);

    if (D_800AC834 > 0) {
        Font_BeginTranslucentTextRendering();
        Font_SetActive(8, -4);

        switch (D_800AC834) {
            case 1:
                Font_Printf(0, 0x64, "S:%5.2f  H:%4d", arg0->modelScale, (s32)arg0->displayObject->unk_024.y);
                break;

            case 2:
                Font_Printf(0, 0x64, "X:%4d  Z:%4d", (s32)arg0->displayObject->unk_024.x, (s32)arg0->displayObject->unk_024.z);
                break;

            case 3:
                Font_Printf(0, 0x64, "A:%04X  F:%3d", *(u16*)&arg0->displayObject->unk_01E.y,
                              arg0->displayObject->unk_040.unk_08 >> 0x10);
                break;
        }
        Font_EndTexturedTextRendering();
    }
    return arg0->image->img_p;
}
