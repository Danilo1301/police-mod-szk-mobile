#pragma once

#include "pch.h"

#include "simpleGta.h"

extern char* (*GetFrameNodeName)(RwFrame* frame);

class VehicleDummy {
public:
    // Pega todos os atomics de um clump
    static std::vector<RpAtomic*> RpClumpGetAllAtomics(RpClump* clump);

    // Pega todos os materiais de uma geometria
    static std::vector<RpMaterial*> RpGeometryGetAllMaterials(RpGeometry* geometry);

    // Retorna todos os frames (dummies) de um veículo
    static std::vector<RwFrame*> GetFramesOnVehicle(CVehicle* vehicle);

    // Procura um dummy pelo nome
    static RwFrame* FindDummy(CVehicle* vehicle, const std::string& dummyName);

private:
    // Função recursiva que varre todos os frames do veículo
    static void FindDummies(CVehicle* vehicle, RwFrame* frame);

    // Guarda os frames temporariamente durante a busca
    static std::vector<RwFrame*> m_Frames;
};

static RwFrame* GetObjectParent(RwObject *obj) {
    return reinterpret_cast<RwFrame *>(rwObjectGetParent(obj));
}

static std::string GetFrameName(RwFrame* frame)
{
    return std::string(GetFrameNodeName(frame));
}

static void HideAllAtomics(RwFrame *frame)
{
	if (!rwLinkListEmpty(&frame->objectList))
	{
		RwObjectHasFrame * atomic;

		RwLLLink * current = rwLinkListGetFirstLLLink(&frame->objectList);
		RwLLLink * end = rwLinkListGetTerminator(&frame->objectList);

		current = rwLinkListGetFirstLLLink(&frame->objectList);
		while (current != end) {
			atomic = rwLLLinkGetData(current, RwObjectHasFrame, lFrame);
			atomic->object.flags &= ~0x4; // clear

			current = rwLLLinkGetNext(current);
		}
	}
	return;
}