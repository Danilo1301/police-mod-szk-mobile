#pragma once

#include <functional>

#include "pch.h"

#include "isautils.h"
extern ISAUtils* sautils;

enum WAIT_FN_STATE {
    WAIT_FN_NONE,
    WAIT_FN_COMPLETED,
    WAIT_FN_CANCELLED
};

struct WaitFunction {
    int timePassed = 0;
    int time = 0;
    WAIT_FN_STATE state = WAIT_FN_STATE::WAIT_FN_NONE;
    std::function<void()> onComplete;
    std::function<void()> onCancel;
    
    bool isTestFunction = false;
    std::function<bool()> testFn;

    bool isConditionFunction = false;
    std::function<void(std::function<void()>, std::function<void()>)> conditionFn;
};

enum class PedType : int
{
    Player1       = 0,
    Player2       = 1,
    PlayerNetwork = 2,
    PlayerUnused  = 3,
    CivMale       = 4,
    CivFemale     = 5,
    Cop           = 6,
    Gang1         = 7,
    Gang2         = 8,
    Gang3         = 9,
    Gang4         = 10,
    Gang5         = 11,
    Gang6         = 12,
    Gang7         = 13,
    Gang8         = 14,
    Gang9         = 15,
    Gang10        = 16,
    Dealer        = 17,
    Emergency     = 18,
    Fireman       = 19,
    Criminal      = 20,
    Bum           = 21,
    Prostitute    = 22,
    Special       = 23,
    Mission1      = 24,
    Mission2      = 25,
    Mission3      = 26,
    Mission4      = 27,
    Mission5      = 28,
    Mission6      = 29,
    Mission7      = 30,
    Mission8      = 31
};

class CleoFunctions {
public:
    static void Update(int dt);

    static WaitFunction* AddWaitFunction(int time, std::function<void()> callback);
    static void RemoveWaitFunction(WaitFunction* waitFunction);

    static WaitFunction* AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback);
    static WaitFunction* AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel);
};

// --------------------------------------------

//0247: load_model 110
static DEFOPCODE(0247, LOAD_MODEL, i);
static void LOAD_MODEL(int modelId)
{
    sautils->ScriptCommand(&scm_LOAD_MODEL, modelId);
}

//01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv);
static int GET_PLAYER_ACTOR(int player)
{
    int playerActor = 0;
    sautils->ScriptCommand(&scm_GET_PLAYER_ACTOR, 0, &playerActor);
    return playerActor;
}

//0457: player $PLAYER_CHAR aiming_at_actor 0@
static DEFOPCODE(0457, PLAYER_AIMING_AT_ACTOR, ii);
static bool PLAYER_AIMING_AT_ACTOR(int player, int _char)
{
    bool result = sautils->ScriptCommand(&scm_PLAYER_AIMING_AT_ACTOR, player, _char);
    return result;
}

//02C1: store_to 127@ 128@ 129@ car_path_coords_closest_to 124@ 125@ 126@
static DEFOPCODE(02C1, GET_CLOSEST_CAR_NODE, fffvvv);
static CVector GET_CLOSEST_CAR_NODE(float x, float y, float z)
{
    CVector result;
    sautils->ScriptCommand(&scm_GET_CLOSEST_CAR_NODE, x, y, z, &result.x, &result.y, &result.z);
    return result;
}

// 0683: attach_car 216@ to_car 196@ with_offset 0.0 -4.6 0.65 rotation 15.0 0.0 0.0 
static DEFOPCODE(0683, ATTACH_CAR_TO_CAR, iiffffff);
static void ATTACH_CAR_TO_CAR(int car, int toCar, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ)
{
    sautils->ScriptCommand(&scm_ATTACH_CAR_TO_CAR, car, toCar, offsetX, offsetY, offsetZ, rotX, rotY, rotZ);
}

//00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
static DEFOPCODE(00A5, CREATE_CAR_AT, ifffv);
static int CREATE_CAR_AT(int modelId, float x, float y, float z)
{
    int car = 0;
    sautils->ScriptCommand(&scm_CREATE_CAR_AT, modelId, x, y, z, &car);
    return car;
}

//01C8: $P2 = create_actor_pedtype 23 model 280 in_car $POLICE_CAR passenger_seat 0 
static DEFOPCODE(01C8, CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, iiiiv);
static int CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(int vehicle, PedType pedType, int modelId, int seatId)
{
    int ped = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, vehicle, (int)pedType, modelId, seatId, &ped);
    return ped;
}

//02D4: car 3@ turn_off_engine 
static DEFOPCODE(02D4, CAR_TURN_OFF_ENGINE, i);
static void CAR_TURN_OFF_ENGINE(int car)
{
    sautils->ScriptCommand(&scm_CAR_TURN_OFF_ENGINE, car);
}

//02C0: store_to 3@ 4@ 5@ ped_path_coords_closest_to 0@ 1@ 2@
static DEFOPCODE(02C0, STORE_PED_PATH_COORDS_CLOSEST_TO, fffvvv);
static CVector STORE_PED_PATH_COORDS_CLOSEST_TO(float x, float y, float z)
{
    CVector result = CVector(0, 0, 0);
    sautils->ScriptCommand(&scm_STORE_PED_PATH_COORDS_CLOSEST_TO, x, y, z, &result.x, &result.y, &result.z);
    return result;
}

static DEFOPCODE(0129, CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, iiiv); //0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
static int CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(int car, PedType pedType, int modelId)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, car, (int)pedType, modelId, &_char);
    return _char;
}

static DEFOPCODE(0186, ADD_BLIP_FOR_CAR, iv); //0186: $60 = create_marker_above_car $59 
static int ADD_BLIP_FOR_CAR(int car)
{
    int blip = 0;
    sautils->ScriptCommand(&scm_ADD_BLIP_FOR_CAR, car, &blip);
    return blip;
}

// 044B: actor %1d male
static DEFOPCODE(044B, ACTOR_MALE, i);
static bool ACTOR_MALE(int actor)
{
    return sautils->ScriptCommand(&scm_ACTOR_MALE, actor);
}

// 044C: actor %1d driving
static DEFOPCODE(044C, ACTOR_DRIVING, i);
static bool ACTOR_DRIVING(int actor)
{
    return sautils->ScriptCommand(&scm_ACTOR_DRIVING, actor);
}

//01B2: give_actor 3@ weapon 22 ammo 10000
static DEFOPCODE(01B2, GIVE_ACTOR_WEAPON, iii);
static void GIVE_ACTOR_WEAPON(int _char, int weaponType, int ammo)
{
    sautils->ScriptCommand(&scm_GIVE_ACTOR_WEAPON, _char, weaponType, ammo);
}

// 02E3: car %1d speed
static DEFOPCODE(02E3, CAR_SPEED, iF);
static float CAR_SPEED(int vehicle)
{
    float speed = 0.0f;
    sautils->ScriptCommand(&scm_CAR_SPEED, vehicle, &speed);
    return speed;
}

// 01BD: put_car_at %1d x %2f y %3f z %4f
static DEFOPCODE(01BD, PUT_CAR_AT, ifff);
static void PUT_CAR_AT(int vehicle, float x, float y, float z)
{
    sautils->ScriptCommand(&scm_PUT_CAR_AT, vehicle, x, y, z);
}

// 0175: set_car_z_angle %1d to %2f
static DEFOPCODE(0175, SET_CAR_Z_ANGLE, if);
static void SET_CAR_Z_ANGLE(int vehicle, float heading)
{
    sautils->ScriptCommand(&scm_SET_CAR_Z_ANGLE, vehicle, heading);
}

// 017A: set_car_immunities %1d BP %2d FP %3d EP %4d CP %5d MP %6d
static DEFOPCODE(017A, SET_CAR_IMMUNITIES, iiiiii);
static void SET_CAR_IMMUNITIES(int vehicle, bool BP, bool FP, bool EP, bool CP, bool MP)
{
    sautils->ScriptCommand(&scm_SET_CAR_IMMUNITIES, vehicle, BP ? 1 : 0, FP ? 1 : 0, EP ? 1 : 0, CP ? 1 : 0, MP ? 1 : 0);
}

// 03AC: load_requested_models
static DEFOPCODE(03AC, LOAD_REQUESTED_MODELS, );
static void LOAD_REQUESTED_MODELS()
{
    sautils->ScriptCommand(&scm_LOAD_REQUESTED_MODELS);
}

// 038B: load_requested_anims
static DEFOPCODE(038B, LOAD_REQUESTED_ANIMS, );
static void LOAD_REQUESTED_ANIMS()
{
    sautils->ScriptCommand(&scm_LOAD_REQUESTED_ANIMS);
}

// 0247: request_model %1d
static DEFOPCODE(0247, REQUEST_MODEL, i);
static void REQUEST_MODEL(int modelId)
{
    sautils->ScriptCommand(&scm_REQUEST_MODEL, modelId);
}

// 0249: release_model %1d
static DEFOPCODE(0249, RELEASE_MODEL, i);
static void RELEASE_MODEL(int modelId)
{
    sautils->ScriptCommand(&scm_RELEASE_MODEL, modelId);
}

//09C7: change_player 0 model_to 280
static DEFOPCODE(09C7, CHANGE_PLAYER_MODEL_TO, ii);
static void CHANGE_PLAYER_MODEL_TO(int player, int modelId)
{
    sautils->ScriptCommand(&scm_CHANGE_PLAYER_MODEL_TO, player, modelId);
}

//04EE: animation "GANGS" loaded 
static DEFOPCODE(04EE, HAS_ANIMATION_LOADED, s);
static bool HAS_ANIMATION_LOADED(const char* animationFile)
{
    bool result = sautils->ScriptCommand(&scm_HAS_ANIMATION_LOADED, animationFile);
    return result;
}

//04ED: load_animation "GANGS"
static DEFOPCODE(04ED, LOAD_ANIMATION, s);
static void LOAD_ANIMATION(const char* animationFile)
{
    sautils->ScriptCommand(&scm_LOAD_ANIMATION, animationFile);
}

//0256: player $PLAYER_CHAR defined 
static DEFOPCODE(0256, PLAYER_DEFINED, i);
static bool PLAYER_DEFINED(int player)
{
    bool result = sautils->ScriptCommand(&scm_PLAYER_DEFINED, 0);
    return result;
}

// 0248: model %1d available
static DEFOPCODE(0248, HAS_MODEL_LOADED, i);
static bool HAS_MODEL_LOADED(int modelId)
{
    return sautils->ScriptCommand(&scm_HAS_MODEL_LOADED, modelId);
}

// 02F6: actor %1d in_zone %2s
static DEFOPCODE(02F6, ACTOR_IN_ZONE, is);
static bool ACTOR_IN_ZONE(int actor, const char* zoneName)
{
    return sautils->ScriptCommand(&scm_ACTOR_IN_ZONE, actor, zoneName);
}

// 0107: player %1d money += %2d
static DEFOPCODE(0107, ADD_PLAYER_MONEY, ii);
static void ADD_PLAYER_MONEY(int player, int amount)
{
    sautils->ScriptCommand(&scm_ADD_PLAYER_MONEY, player, amount);
}

// 0109: player %1d money = %2d
static DEFOPCODE(0109, SET_PLAYER_MONEY, ii);
static void SET_PLAYER_MONEY(int player, int amount)
{
    sautils->ScriptCommand(&scm_SET_PLAYER_MONEY, player, amount);
}

// 0108: player %1d money
static DEFOPCODE(0108, GET_PLAYER_MONEY, iI);
static int GET_PLAYER_MONEY(int player)
{
    int money = 0;
    sautils->ScriptCommand(&scm_GET_PLAYER_MONEY, player, &money);
    return money;
}

// 01C2: remove_references_to_actor %1d
static DEFOPCODE(01C2, REMOVE_REFERENCES_TO_ACTOR, i);
static void REMOVE_REFERENCES_TO_ACTOR(int actor)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_ACTOR, actor);
}

// 01C3: remove_references_to_car %1d
static DEFOPCODE(01C3, REMOVE_REFERENCES_TO_CAR, i);
static void REMOVE_REFERENCES_TO_CAR(int vehicle)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_CAR, vehicle);
}

static DEFOPCODE(01EA, CAR_MAX_PASSENGERS, iv); //01EA: 68@ = car 67@ max_passengers 
static int CAR_MAX_PASSENGERS(int car)
{
    int maxPassengers = 0;
    sautils->ScriptCommand(&scm_CAR_MAX_PASSENGERS, car, &maxPassengers);
    return maxPassengers;
}

//0432: 19@ = get_actor_handle_from_car $47 passenger_seat 0 
static DEFOPCODE(0432, GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, iiv);
static int GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(int car, int seatId)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, car, seatId, &_char);
    return _char;
}

//0431: car $47 passenger_seat_free 0
static DEFOPCODE(0431, CAR_PASSENGER_SEAT_FREE, ii);
static bool CAR_PASSENGER_SEAT_FREE(int car, int seatId)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_CAR_PASSENGER_SEAT_FREE, car, seatId);
    return result;
}

static DEFOPCODE(00A7, CAR_DRIVE_TO, ifff); //00A7: car 7@ drive_to 0@ 1@ 2@
static void CAR_DRIVE_TO(int car, float x, float y, float z)
{
    sautils->ScriptCommand(&scm_CAR_DRIVE_TO, car, x, y, z);
}

// 01C4: remove_references_to_object %1d
static DEFOPCODE(01C4, REMOVE_REFERENCES_TO_OBJECT, i);
static void REMOVE_REFERENCES_TO_OBJECT(int obj)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_OBJECT, obj);
}

// 01C7: remove_references_to_car %1d
static DEFOPCODE(01C7, REMOVE_REFERENCES_TO_HELI, i);
static void REMOVE_REFERENCES_TO_HELI(int heli)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_HELI, heli);
}

// 01C8: remove_references_to_pickup %1d
static DEFOPCODE(01C8, REMOVE_REFERENCES_TO_PICKUP, i);
static void REMOVE_REFERENCES_TO_PICKUP(int pickup)
{
    sautils->ScriptCommand(&scm_REMOVE_REFERENCES_TO_PICKUP, pickup);
}

//04C4: store_coords_to 4@ 5@ 6@ from_actor $PLAYER_ACTOR with_offset 1.0 2.0 0.0 
static DEFOPCODE(04C4, STORE_COORDS_FROM_ACTOR_WITH_OFFSET, ifffvvv);
static void STORE_COORDS_FROM_ACTOR_WITH_OFFSET(int _char, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_STORE_COORDS_FROM_ACTOR_WITH_OFFSET, _char, offsetX, offsetY, offsetZ, x, y, z);
}

//00DF: actor $PLAYER_ACTOR driving 
static DEFOPCODE(00DF, IS_CHAR_IN_ANY_CAR, i);
static bool IS_CHAR_IN_ANY_CAR(int _char)
{
    bool result = sautils->ScriptCommand(&scm_IS_CHAR_IN_ANY_CAR, _char);
    return result;
}

//0811: $47 = actor $PLAYER_ACTOR used_car 
static DEFOPCODE(0811, ACTOR_USED_CAR, iv);
static int ACTOR_USED_CAR(int _char)
{
    int car = 0;
    sautils->ScriptCommand(&scm_ACTOR_USED_CAR, _char, &car);
    return car;
}

//0407: store_coords_to 128@ 138@ 148@ from_car 551@ with_offset -0.337 1.566 0.657 
static DEFOPCODE(0407, STORE_COORDS_FROM_CAR_WITH_OFFSET, ifffvvv);
static void STORE_COORDS_FROM_CAR_WITH_OFFSET(int car, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    sautils->ScriptCommand(&scm_STORE_COORDS_FROM_CAR_WITH_OFFSET, car, offsetX, offsetY, offsetZ, x, y, z);
}

//0812: AS_actor 0@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1
static DEFOPCODE(0812, PERFORM_ANIMATION_AS_ACTOR, issfbbbbi);
static void PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time)
{
    sautils->ScriptCommand(&scm_PERFORM_ANIMATION_AS_ACTOR, _char, animationName, animationFile, frameDelta, loop, lockX, lockY, lockF, time);
}

//009A: 6@ = create_actor_pedtype 20 model #DNFYLC at 3@ 4@ 5@
static DEFOPCODE(009A, CREATE_ACTOR_PEDTYPE, iifffv);
static int CREATE_ACTOR_PEDTYPE(PedType pedType, int modelId, float x, float y, float z)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_CREATE_ACTOR_PEDTYPE, (int)pedType, modelId, x, y, z, &_char);
    return _char;
}

//0687: clear_actor $PLAYER_ACTOR task
static DEFOPCODE(0687, CLEAR_ACTOR_TASK, i);
static void CLEAR_ACTOR_TASK(int _char)
{
    sautils->ScriptCommand(&scm_CLEAR_ACTOR_TASK, _char);
}

//0611: actor 2@ performing_animation "LRGIRL_IDLE_TO_L0"
static DEFOPCODE(0611, ACTOR_PERFORMING_ANIMATION, is);
static bool ACTOR_PERFORMING_ANIMATION(int _char, const char* animationName)
{
    bool result = sautils->ScriptCommand(&scm_ACTOR_PERFORMING_ANIMATION, _char, animationName);
    return result;
}

//0603: AS_actor @3 goto_point_any_means 2493.82 -1669.91 12.8 mode 7 use_car -1 
static DEFOPCODE(0603, TASK_GO_TO_COORD_ANY_MEANS, ifffib);
static void TASK_GO_TO_COORD_ANY_MEANS(int actor, float x, float y, float z, int mode, bool useCar)
{
    sautils->ScriptCommand(&scm_TASK_GO_TO_COORD_ANY_MEANS, actor, x, y, z, mode, useCar ? 1 : 0);
}

//0918: set_car 3@ engine_operation 1 
static DEFOPCODE(0918, SET_CAR_ENGINE_OPERATION, ib);
static void SET_CAR_ENGINE_OPERATION(int car, bool state)
{
    sautils->ScriptCommand(&scm_SET_CAR_ENGINE_OPERATION, car, state);
}

//00AE: set_car 3@ traffic_behaviour_to 2
static DEFOPCODE(00AE, SET_CAR_TRAFFIC_BEHAVIOUR, ii);
static void SET_CAR_TRAFFIC_BEHAVIOUR(int car, int drivingStyle)
{
    sautils->ScriptCommand(&scm_SET_CAR_TRAFFIC_BEHAVIOUR, car, drivingStyle);
}

//00A8: set_car 52@ to_psycho_driver 
static DEFOPCODE(00A8, SET_CAR_TO_PSYCHO_DRIVER, i);
static void SET_CAR_TO_PSYCHO_DRIVER(int car)
{
    sautils->ScriptCommand(&scm_SET_CAR_TO_PSYCHO_DRIVER, car);
}

//00AD: set_car 3@ max_speed_to 50.0
static DEFOPCODE(00AD, SET_CAR_MAX_SPEED, if);
static void SET_CAR_MAX_SPEED(int car, float maxSpeed)
{
    sautils->ScriptCommand(&scm_SET_CAR_MAX_SPEED, car, maxSpeed);
}

//046C: 133@ = car 50@ driver 
static DEFOPCODE(046C, GET_DRIVER_OF_CAR, iv);
static int GET_DRIVER_OF_CAR(int car)
{
    int _char = 0;
    sautils->ScriptCommand(&scm_GET_DRIVER_OF_CAR, car, &_char);
    return _char;
}

//0633: AS_actor 4@ exit_car
static DEFOPCODE(0633, EXIT_CAR_AS_ACTOR, i);
static void EXIT_CAR_AS_ACTOR(int _actor)
{
    sautils->ScriptCommand(&scm_EXIT_CAR_AS_ACTOR, _actor);
}

//05CB: AS_actor 21@ enter_car 0@ as_driver 20000 ms
static DEFOPCODE(05CB, ENTER_CAR_AS_DRIVER_AS_ACTOR, iii);
static void ENTER_CAR_AS_DRIVER_AS_ACTOR(int _char, int vehicle, int time)
{
    sautils->ScriptCommand(&scm_ENTER_CAR_AS_DRIVER_AS_ACTOR, _char, vehicle, time);
}

//05CA: AS_actor 3@ enter_car 7@ passenger_seat 1 time 10000
static DEFOPCODE(05CA, ACTOR_ENTER_CAR_PASSENGER_SEAT, iiii);
static void ACTOR_ENTER_CAR_PASSENGER_SEAT(int _char, int vehicle, int time, int seatId)
{
    sautils->ScriptCommand(&scm_ACTOR_ENTER_CAR_PASSENGER_SEAT, _char, vehicle, time, seatId);
}

//0187: 47@ = create_marker_above_actor 75@ 
static DEFOPCODE(0187, ADD_BLIP_FOR_CHAR, iv);
static int ADD_BLIP_FOR_CHAR(int _char)
{
    int blip = 0;
    sautils->ScriptCommand(&scm_ADD_BLIP_FOR_CHAR, _char, &blip);
    return blip;
}

//0164: disable_marker $482
static DEFOPCODE(0164, DISABLE_MARKER, i);
static void DISABLE_MARKER(int blip)
{
    sautils->ScriptCommand(&scm_DISABLE_MARKER, blip);
}

// --------------------------------------------

static void WAIT(int time, std::function<void()> callback)
{
    CleoFunctions::AddWaitFunction(time, callback);
}

// --------------------------------------------

static int GetPlayerActor()
{
    return GET_PLAYER_ACTOR(0);
}

static CVector GetPedPositionWithOffset(int hPed, CVector offset)
{
    float x = 0, y = 0, z = 0;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hPed, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

static CVector GetPedPosition(int hPed)
{
    return GetPedPositionWithOffset(hPed, CVector(0, 0, 0));
}

static int GetVehiclePedIsUsing(int hPed)
{
    if(!IS_CHAR_IN_ANY_CAR(hPed)) return 0;

    return ACTOR_USED_CAR(hPed);
}

static void ClearPedAnimations(int hPed)
{
    REMOVE_REFERENCES_TO_ACTOR(hPed);
    PERFORM_ANIMATION_AS_ACTOR(hPed, "hndshkfa_swt", "gangs", 500.0f, 0, 0, 0, 0, 1);
}

static CVector GetPlayerPosition()
{
    return GetPedPosition(GetPlayerActor());
}

static CVector GetPlayerPositionInForward(float distanceY)
{
    return GetPedPositionWithOffset(GetPlayerActor(), CVector(0, distanceY, 0));
}

static CVector GetCarPositionWithOffset(int hVehicle, CVector offset)
{
    float x = 0, y = 0, z = 0;
    STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

static CVector GetCarPosition(int hVehicle)
{
    return GetCarPositionWithOffset(hVehicle, CVector(0, 0, 0));
}

static double DistanceFromPed(int hPed, CVector position)
{
    auto pedPosition = GetPedPosition(hPed);
    auto distance = distanceBetweenPoints(pedPosition, position);
    return distance;
}

static double DistanceFromVehicle(int hCar, CVector position)
{
    auto carPosition = GetCarPosition(hCar);
    auto distance = distanceBetweenPoints(carPosition, position);
    return distance;
}