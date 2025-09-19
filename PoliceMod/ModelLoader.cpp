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

    if (CleoFunctions::MODEL_AVAILABLE(modelId)) {
        ModelLoaded(modelId);
        return;
    }


    CleoFunctions::LOAD_MODEL(modelId);
    CleoFunctions::LOAD_REQUESTED_MODELS();

    if (CleoFunctions::MODEL_AVAILABLE(modelId)) {
        ModelLoaded(modelId);
        return;
    }

    CleoFunctions::AddWaitForFunction(
        [modelId]() { return CleoFunctions::MODEL_AVAILABLE(modelId); },
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