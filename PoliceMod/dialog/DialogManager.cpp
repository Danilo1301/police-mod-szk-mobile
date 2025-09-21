#include "DialogManager.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

bool renderDialog = false;
IContainer* dialogContainer = NULL;
ILabel* dialogLabel = NULL;

std::string DialogManager::currentId = "start";
std::stack<std::string> DialogManager::history;
std::vector<Dialogue> DialogManager::loadedDialogs;
Dialogue* DialogManager::currentDialogue = nullptr;

void DialogManager::Initialize()
{
    LoadDialogs();

    auto container = dialogContainer = menuSZK->CreateContainer();
    container->localOffset = CVector2D(0, -50);
    container->size = CVector2D(1200, 150);
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Bottom;
    container->drawBackground = true;
    container->backgroundColor = CRGBA(34, 34, 34);
    menuSZK->GetScreenContainer()->AddChild(container);

    {
        auto label = dialogLabel = menuSZK->CreateLabel();
        label->text = "Linha 1\nlinha 2\nlinha 3";
        label->textHorizontalAlign = HorizontalAlign::Middle;
        label->textVerticalAlign = VerticalAlign::Middle;
        label->fontSize = 0.8f;

        auto labelContainer = label->GetContainer();
        labelContainer->horizontalAlign = HorizontalAlign::Middle;
        labelContainer->verticalAlign = VerticalAlign::Middle;
        labelContainer->drawBackground = false;
        labelContainer->canClickThrough = true;
        container->AddChild(labelContainer);
    }
    
    auto containerTitle = menuSZK->CreateContainer();
    containerTitle->localOffset = CVector2D(0, -50);
    containerTitle->size = CVector2D(400, 50);
    containerTitle->horizontalAlign = HorizontalAlign::Left;
    containerTitle->verticalAlign = VerticalAlign::Top;
    containerTitle->drawBackground = true;
    containerTitle->backgroundColor = CRGBA(242, 196, 16);
    container->AddChild(containerTitle);

    dialogContainer->onClick->Add([](IContainer* c) {
        OnClickAnywhere();
    });
}

void DialogManager::Update()
{
    if (dialogContainer)
        dialogContainer->isVisible = renderDialog;

    if (currentDialogue)
    {
        auto node = currentDialogue->FindNode(currentId);
        if (node)
        {
            dialogLabel->text = node->npcText;
        }
    }
}

void DialogManager::BeginDialogue(Dialogue* dialogue)
{
    currentDialogue = dialogue;
    renderDialog = true;

    currentId = "start";   // ID inicial
    history = {};
    Update();
}

void DialogManager::EndDialogue()
{
    currentDialogue = NULL;
    renderDialog = false;
    history = {};
}

void DialogManager::OnClickAnywhere()
{
    if (!currentDialogue) return;

    auto node = currentDialogue->FindNode(currentId);
    if (!node) 
    {
        EndDialogue();
        debug->AddLine("~r~node not found: " + currentId);
        return;
    }

    debug->AddLine("node: " + currentId);

    Update();

    // Avança automaticamente se houver nextId
    if (!node->nextId.empty())
    {
        currentId = node->nextId;
        renderDialog = true;
        debug->AddLine("auto advance to: " + currentId);
        Update();
        return;
    }

    // Se não houver opções, termina diálogo
    if (node->options.empty())
    {
        EndDialogue();
        debug->AddLine("~y~dialogue ended: no options");
        return;
    }
    else
    {
        auto option0 = node->options[0];
        if (!option0.nextId.empty())
        {
            history.push(currentId);
            debug->AddLine("adding to history...");
        }
    }

    debug->AddLine("showing options...");
    ShowOptionsForNode(node); // 👈 centralizado aqui
}

void DialogManager::ShowOptionsForNode(DialogueNode* node)
{
    if (!node || node->options.empty())
        return;

    renderDialog = false;

    auto window = menuSZK->CreateWindow(400, 30, 900, "Dialogar");
    auto closeWindow = [w = window]() { w->Close(); };

    for (size_t i = 0; i < node->options.size(); i++)
    {
        auto& opt = node->options[i];
        std::string buttonText = opt.used ? ("~y~(usada) ~w~" + opt.text) : opt.text;

        auto button = window->AddButton(buttonText);

        button->onClick->Add([i, closeWindow](IContainer*)
        {
            auto* nodePtr = currentDialogue->FindNode(currentId);
            if (!nodePtr) return;

            auto& opt = nodePtr->options[i];

            opt.used = true;
            currentId = opt.nextId;
            
            if (ContainsInHistory(currentId))
            {
                auto* nextNode = currentDialogue->FindNode(currentId);
                if (nextNode && !nextNode->options.empty())
                {
                    ShowOptionsForNode(nextNode); // já mostra opções
                }
            }
            else
            {
                renderDialog = true;
                Update();
            }

            closeWindow();
        });
    }

    auto closeButton = window->AddButton("~r~Encerrar");
    closeButton->onClick->Add([closeWindow](IContainer*) 
    {
        closeWindow();
        EndDialogue();
    });
}

void DialogManager::LoadDialogs()
{
    loadedDialogs.clear();

    std::string dialogsFolder = getPathFromModFolder("data/dialogs");

    // Iterar sobre todos os arquivos da pasta
    for (const auto& entry : std::filesystem::directory_iterator(dialogsFolder))
    {
        if (!entry.is_regular_file()) continue;

        std::string path = entry.path().string();

        // Filtrar apenas .json
        if (entry.path().extension() == ".json")
        {
            Dialogue dlg = LoadDialogFromFile(path);
            dlg.name = entry.path().filename().string();

            debug->AddLine("loaded dialog: " + dlg.name);

            loadedDialogs.push_back(dlg);
        }

    }
}

Dialogue DialogManager::LoadDialogFromFile(std::string src)
{
    Json::Value root = jsonReadFile(src);

    Dialogue dialogue;

    // Variáveis
    dialogue.variables.isArmed = root["variables"].get("isArmed", false).asBool();
    dialogue.variables.isDocummentExpired = root["variables"].get("isDocummentExpired", false).asBool();
    dialogue.variables.hasCNH = root["variables"].get("hasCNH", false).asBool();

    // Nós
    const Json::Value nodes = root["nodes"];
    for (const auto& nodeJson : nodes)
    {
        DialogueNode node;
        node.id = nodeJson.get("id", "").asString();
        node.npcText = nodeJson.get("npcText", "").asString();
        node.nextId = nodeJson.get("nextId", "").asString();

        const Json::Value options = nodeJson["options"];
        for (const auto& optJson : options)
        {
            DialogueOption opt;
            opt.text = optJson.get("text", "").asString();
            opt.nextId = optJson.get("nextId", "").asString();
            node.options.push_back(opt);
        }

        dialogue.nodes.push_back(node);
    }
    return dialogue;
}