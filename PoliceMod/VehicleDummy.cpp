#include "VehicleDummy.h"

extern RpClump*     (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry*  (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);

std::vector<RwFrame*> VehicleDummy::m_Frames;

// ---------------------------------------------------
// Pega todos os atomics de um clump
// ---------------------------------------------------
std::vector<RpAtomic*> VehicleDummy::RpClumpGetAllAtomics(RpClump* clump)
{
    std::vector<RpAtomic*> atomics;

    RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) {
        auto* list = reinterpret_cast<std::vector<RpAtomic*>*>(data);
        list->push_back(atomic);
        return atomic;
    }, &atomics);

    return atomics;
}

// ---------------------------------------------------
// Pega todos os materiais de uma geometria
// ---------------------------------------------------
std::vector<RpMaterial*> VehicleDummy::RpGeometryGetAllMaterials(RpGeometry* geometry)
{
    std::vector<RpMaterial*> materials;

    RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* data) {
        auto* list = reinterpret_cast<std::vector<RpMaterial*>*>(data);
        list->push_back(material);
        return material;
    }, &materials);

    return materials;
}

// ---------------------------------------------------
// Retorna todos os frames (dummies) de um veículo
// ---------------------------------------------------
std::vector<RwFrame*> VehicleDummy::GetFramesOnVehicle(CVehicle* vehicle)
{
    m_Frames.clear();
    FindDummies(vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);
    return m_Frames;
}

// ---------------------------------------------------
// Procura um dummy pelo nome
// ---------------------------------------------------
RwFrame* VehicleDummy::FindDummy(CVehicle* vehicle, const std::string& dummyName)
{
    auto frames = GetFramesOnVehicle(vehicle);

    for (auto frame : frames)
    {
        std::string frameName = to_lower(GetFrameNodeName(frame));
        if (frameName == to_lower(dummyName))
            return frame;
    }
    return nullptr;
}

// ---------------------------------------------------
// Função recursiva que varre todos os frames
// ---------------------------------------------------
void VehicleDummy::FindDummies(CVehicle* vehicle, RwFrame* frame)
{
    if (!frame) return;

    if (frame->child) FindDummies(vehicle, frame->child);
    if (frame->next)  FindDummies(vehicle, frame->next);

    if (std::find(m_Frames.begin(), m_Frames.end(), frame) == m_Frames.end())
        m_Frames.push_back(frame);
}