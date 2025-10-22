#include "Pullover.h"

#include "Peds.h"
#include "CleoFunctions.h"
#include "Criminals.h"
#include "ModelLoader.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "TopMessage.h"
#include "ScriptTask.h"
#include "Chase.h"
#include "RadioWindow.h"
#include "Escort.h"

int aimingPed = NO_PED_FOUND;

void Pullover::OnClickWidget()
{
    fileLog->Log("Clicked pullover widget");

    int playerActor = GET_PLAYER_ACTOR(0);

    if(IS_CHAR_IN_ANY_CAR(playerActor))
    {
        TryPulloverFromVehicle();
    } else {
        TryPulloverOnFoot();
    }

    blockInput = !blockInput;
}

void Pullover::TryPulloverFromVehicle()
{
    TryPulloverClosestVehicle();
}

void Pullover::TryPulloverOnFoot()
{
    fileLog->Log("try pullover on foot");

    aimingPed = FindAimingPed();

    if(aimingPed == NO_PED_FOUND)
    {
        menuDebug->AddLine("~r~no ped found");

        TryPulloverClosestVehicle();
        return;
    }

    auto ped = Peds::GetPed(aimingPed);

    PulloverPed(ped);
}

void Pullover::TryPulloverClosestVehicle()
{
    int playerActor = GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if(IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    Vehicle* closestCar = Vehicles::GetClosestVehicleNotPlayer(CVector(x, y, z), GetPlayerPosition(), 5.0f);

    if(closestCar == NULL)
    {
        if(IS_CHAR_IN_ANY_CAR(playerActor))
        {
            BottomMessage::SetMessage(GetTranslatedText("pullover_from_vehicle_fail"), 3000);
        } else {
            BottomMessage::SetMessage(GetTranslatedText("pullover_on_foot_fail"), 3000);
        }

        return;
    }

    PulloverVehicle(closestCar);
}

void Pullover::PulloverPed(Ped* ped)
{
    if(ped->flags.isInconcious)
    {
        menuDebug->AddLine("~g~pull over ped incounsious");

        ped->flags.showWidget = true;
        return;
    }

    menuDebug->AddLine("~g~pull over ped");

    if(ped->flags.willSurrender == false)
    {
        ped->ShowBlip(COLOR_CRIMINAL);
        Criminals::AddCriminal(ped);

        ModelLoader::AddModelToLoad(346);
        ModelLoader::LoadAll([ped]() {
            GIVE_ACTOR_WEAPON(ped->ref, 22, 500);
        });

        KILL_ACTOR(ped->ref, GetPlayerActor());

        return;
    }

    ped->flags.hasSurrended = true;
    ped->flags.showWidget = true;
    ped->SetCanDoHandsup();
    ped->ShowBlip(COLOR_CRIMINAL);
    Criminals::AddCriminal(ped);
}

void Pullover::FreePed(Ped* ped)
{
    menuDebug->AddLine("~g~free ped");

    ped->flags.showWidget = false;
    ped->flags.hasSurrended = false;
    ped->ClearAnim();
    ped->HideBlip();
    Criminals::RemoveCriminal(ped);
}

void Pullover::PulloverVehicle(Vehicle* vehicle)
{
    BottomMessage::SetMessage(GetTranslatedText("officer_pullover_vehicle"), 3000);

    if(vehicle->HasDriver())
    {
        auto driver = Peds::GetPed(vehicle->GetCurrentDriver());

        if(driver->flags.willSurrender == false)
        {
            BottomMessage::SetMessage("~r~O suspeito fugiu!", 3000);
            
            Chase::StartChaseWithVehicle(vehicle);

            return;
        }
    }

    vehicle->ShowBlip(COLOR_CRIMINAL);
    vehicle->flags.showWidget = true;

    WAIT(1000, [vehicle]() {
        CAR_TURN_OFF_ENGINE(vehicle->ref);
    });

    WAIT(2500, []() {
        BottomMessage::SetMessage(GetTranslatedText("pullover_get_close_to_vehicle"), 3000);
    });
}

void Pullover::FreeVehicle(Vehicle* vehicle)
{
    vehicle->HideBlip();
    vehicle->flags.showWidget = false;

    vehicle->MakeOwnersEnter();

    CleoFunctions::AddWaitForFunction("passengers_enter_vehicle_to_free", [vehicle] () {
        if(!Vehicles::IsValid(vehicle)) return true;

        auto ownersCount = vehicle->GetOwners().size();
        auto ocuppantsCount = vehicle->GetCurrentOccupants().size();

        if(ocuppantsCount >= ownersCount) return true;

        return false;
    }, [vehicle] () {
        if(!Vehicles::IsValid(vehicle)) return;

        auto driverRef = vehicle->GetCurrentDriver();
        
        if(!ACTOR_DEFINED(driverRef)) return;

        auto driver = Peds::GetPed(driverRef);
        driver->StartDrivingRandomly();

        auto ocuppants = vehicle->GetCurrentOccupants();
            
        for(auto pedRef : ocuppants)
        {
            auto ped = Peds::GetPed(pedRef);

            Criminals::RemoveCriminal(ped);
            ped->HideBlip();
            ped->flags.hasSurrended = false;
            ped->flags.showWidget = false;
        }
    });
}

int Pullover::FindAimingPed()
{
    for(int ped = 1; ped < 35584; ped++)
    {
        if(PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            aimingPed = ped;
            return ped;
        }
    }
    return NO_PED_FOUND;
}

void Pullover::OpenPedMenu(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("pullover_title"));
    
    if(ped->flags.isInconcious)
    {
        {
            auto button = window->AddButton(GetTranslatedText("reanimate_ped"));
            button->onClick->Add([window, ped]() {
                window->Close();

                ped->Reanimate();
            });
        }
    }

    if(ped->flags.isInconcious == false)
    {
        {
            auto button = window->AddButton(GetTranslatedText("hands_on_head"));
            button->onClick->Add([window, ped]() {
                window->Close();

                ped->SetAnim("handscower", "PED");
            });
        }

        {
            auto button = window->AddButton(GetTranslatedText("hands_behind"));
            button->onClick->Add([window, ped]() {
                window->Close();

                ped->SetAnim("bomber", "PED");
            });
        }

        {
            auto button = window->AddButton(GetTranslatedText("escort_ped"));
            button->onClick->Add([window, ped]() {
                window->Close();

                if(Escort::IsEscortingSomeone())
                {
                    BottomMessage::SetMessage("~r~Voce ja esta escoltando", 3000);
                    return;
                }

                Escort::EscortPed(ped);
            });
        }
    }

    if(ped->flags.isInconcious == false)
    {
        if(ped->vehicleOwned > 0)
        {
            auto vehicle = Vehicles::GetVehicle(ped->vehicleOwned);

            auto button = window->AddButton("~r~" + GetTranslatedText("free_vehicle"));
            button->onClick->Add([window, vehicle]() {
                window->Close();

                if(!Vehicles::IsValid(vehicle))
                {
                    BottomMessage::SetMessage("~r~An error ocurred", 2000);
                    return;
                }

                FreeVehicle(vehicle);
            });
        } else {
            auto button = window->AddButton("~r~" + GetTranslatedText("free_ped"));
            button->onClick->Add([window, ped]() {
                window->Close();

                FreePed(ped);
            });
        }
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void Pullover::OpenVehicleMenu(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("pullover_title"));
    
    bool vehicleIsEmpty = vehicle->GetCurrentOccupants().size() == 0;

    if(vehicleIsEmpty)
    {
        auto button = window->AddButton(GetTranslatedText("call_tow_truck"));
        button->onClick->Add([window, vehicle]() {
            window->Close();
            
            auto owners = vehicle->GetOwners();

            for(auto pedRef : owners)
            {
                auto ped = Peds::GetPed(pedRef);
                ped->vehicleOwned = -1;
                ped->UpdateSeatPosition();
            }

            vehicle->flags.showWidget = false;

            CallTowTruck(vehicle);
        });
    }

    if(vehicleIsEmpty == false)
    {
        auto button = window->AddButton(GetTranslatedText("ask_leave_vehicle"));
        button->onClick->Add([window, vehicle]() {
            window->Close();
            
            auto ocuppants = vehicle->GetCurrentOccupants();
            
            vehicle->SetOwners();
            vehicle->MakeOccupantsLeave();

            for(auto pedRef : ocuppants)
            {
                auto ped = Peds::GetPed(pedRef);

                Criminals::AddCriminal(ped);
                ped->ShowBlip(COLOR_CRIMINAL);
                ped->flags.hasSurrended = true;
                ped->flags.showWidget = true;
                ped->SetCanDoHandsup();
            }
        });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("free_vehicle"));
        button->onClick->Add([window, vehicle]() {
            window->Close();

            FreeVehicle(vehicle);
        });
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void Pullover::CallTowTruck(Vehicle* vehicle)
{
    fileLog->Log("Pullover: CallTowTruck");

    int towModelId = 578;
    int driverId = 50;

    ModelLoader::AddModelToLoad(towModelId);
    ModelLoader::AddModelToLoad(driverId);

    ModelLoader::LoadAll([towModelId, driverId, vehicle]() {
        
        fileLog->Log("spawning tow truck...");

        auto playerPosition = GetPlayerPosition();
        auto spawnPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y + 100, playerPosition.z);

        auto towRef = CREATE_CAR_AT(towModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto towTruck = Vehicles::RegisterVehicle(towRef);

        towTruck->ShowBlip(COLOR_YELLOW);

        CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(towRef, PedType::CivFemale, driverId);

        auto targetPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y, playerPosition.z);

        ScriptTask* taskDrive = new ScriptTask("drive");

        taskDrive->onBegin = [towRef, targetPosition]() {
            SET_CAR_MAX_SPEED(towRef, 20.0f);
            SET_CAR_TRAFFIC_BEHAVIOUR(towRef, DrivingMode::AvoidCars);
            CAR_DRIVE_TO(towRef, targetPosition.x, targetPosition.y, targetPosition.z);
        };
        taskDrive->onExecute = [towTruck, targetPosition]() {
            
            if(!Vehicles::IsValid(towTruck)) return SCRIPT_CANCEL;

            if(RadioWindow::m_cancelServices) return SCRIPT_CANCEL;

            auto distance = DistanceFromVehicle(towTruck->ref, targetPosition);

            if(distance < 10.0f) return SCRIPT_SUCCESS;

            return SCRIPT_KEEP_GOING;
        };
        taskDrive->onCancel = [towTruck]() {
            TopMessage::ClearMessage();

            if(!Vehicles::IsValid(towTruck)) return;

            BottomMessage::SetMessage("Guincho cancelado!", 3000);

            if(Vehicles::IsValid(towTruck))
            {
                towTruck->DestroySelfAndPeds();
            }
        };
        taskDrive->onComplete = [vehicle, towTruck]() {
            
            TopMessage::ClearMessage();

            //

            CVector offset = CVector(0, -0.5, 0.6);

            if(Vehicles::IsValid(vehicle))
            {
                ATTACH_CAR_TO_CAR(vehicle->ref, towTruck->ref, offset.x, offset.y, offset.z, 0, 0, 0);
            }

            vehicle->HideBlip();

            //

            ScriptTask* taskLeave = new ScriptTask("task_leave");
            taskLeave->onBegin = [towTruck]() {
                ScriptTask::MakeVehicleLeave(towTruck->ref);
            };
            taskLeave->onExecute = [towTruck]() {
                if(!Vehicles::IsValid(towTruck)) return SCRIPT_CANCEL;

                auto towPosition = GetCarPosition(towTruck->ref);
                auto distance = DistanceFromPed(GetPlayerActor(), towPosition);

                menuDebug->AddLine("leave, " + std::to_string(distance));

                if(distance > 120.0f) return SCRIPT_SUCCESS;

                return SCRIPT_KEEP_GOING;
            };
            taskLeave->onComplete = [vehicle, towTruck]() {
                //on complete

                if(Vehicles::IsValid(vehicle))
                {
                    vehicle->DestroySelfAndPeds();
                }
                
                if(Vehicles::IsValid(towTruck))
                {
                    towTruck->DestroySelfAndPeds();
                }
            };
            taskLeave->Start();

        };
        taskDrive->Start();

        TopMessage::SetMessage(GetTranslatedText("wait_for_tow_truck"));
    });
}