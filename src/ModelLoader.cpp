#include "ModelLoader.h"

#include "CleoFunctions.h"

std::vector<int> ModelLoader::modelsToLoad;
std::function<void()> ModelLoader::onLoadAllCallback;

void ModelLoader::AddModelToLoad(int modelId) {
    modelsToLoad.push_back(modelId);
}

void ModelLoader::LoadAll(std::function<void()> callback) {
    onLoadAllCallback = callback;
    LoadNextModel();
}

void ModelLoader::LoadNextModel()
{
    if (modelsToLoad.empty()) {
        if (onLoadAllCallback) onLoadAllCallback();
        return;
    }

    int modelId = modelsToLoad.front();

    if (HAS_MODEL_LOADED(modelId)) {
        ModelLoaded(modelId);
        return;
    }


    LOAD_MODEL(modelId);
    LOAD_REQUESTED_MODELS();

    if (HAS_MODEL_LOADED(modelId)) {
        ModelLoaded(modelId);
        return;
    }

    CleoFunctions::AddWaitForFunction("modelloader_load",
        [modelId]() { return HAS_MODEL_LOADED(modelId); },
        [modelId]() { ModelLoaded(modelId); }
    );
}

void ModelLoader::ModelLoaded(int modelId)
{
    // remove o primeiro da fila
    if (!modelsToLoad.empty() && modelsToLoad.front() == modelId)
        modelsToLoad.erase(modelsToLoad.begin());

    // carrega o próximo
    LoadNextModel();
}