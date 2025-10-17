#pragma once

#include "pch.h"

enum WAIT_FN_STATE {
    WAIT_FN_NONE,
    WAIT_FN_COMPLETED,
    WAIT_FN_CANCELLED
};

struct WaitFunction {
    int timePassed = 0;
    int time = 0;
    std::string name = "function";
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

    static WaitFunction* AddWaitForFunction(std::string name, std::function<bool()> testFn, std::function<void()> callback);
    static WaitFunction* AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel);
};

// --------------------------------------------

template <typename... Args>
inline int CallScriptCommand(const SCRIPT_COMMAND* cmd, Args... args)
{
    return sautils->ScriptCommand(cmd, args...);
}

template <typename... Args>
inline int CallScriptCommand_NoLog(const SCRIPT_COMMAND* cmd, Args... args)
{
    return sautils->ScriptCommand(cmd, args...);
}

//0247: load_model 110
static DEFOPCODE(0247, LOAD_MODEL, i);
inline void LOAD_MODEL(int modelId)
{
    CallScriptCommand(&scm_LOAD_MODEL, modelId);
}

//01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR 
static DEFOPCODE(01F5, GET_PLAYER_ACTOR, iv);
inline int GET_PLAYER_ACTOR(int player)
{
    int playerActor = 0;
    CallScriptCommand_NoLog(&scm_GET_PLAYER_ACTOR, 0, &playerActor);
    return playerActor;
}

//0457: player $PLAYER_CHAR aiming_at_actor 0@
static DEFOPCODE(0457, PLAYER_AIMING_AT_ACTOR, ii);
inline bool PLAYER_AIMING_AT_ACTOR(int player, int _char)
{
    bool result = CallScriptCommand_NoLog(&scm_PLAYER_AIMING_AT_ACTOR, player, _char);
    return result;
}

//02C1: store_to 127@ 128@ 129@ car_path_coords_closest_to 124@ 125@ 126@
static DEFOPCODE(02C1, GET_CLOSEST_CAR_NODE, fffvvv);
inline CVector GET_CLOSEST_CAR_NODE(float x, float y, float z)
{
    CVector result;
    CallScriptCommand_NoLog(&scm_GET_CLOSEST_CAR_NODE, x, y, z, &result.x, &result.y, &result.z);
    return result;
}

// 0683: attach_car 216@ to_car 196@ with_offset 0.0 -4.6 0.65 rotation 15.0 0.0 0.0 
static DEFOPCODE(0683, ATTACH_CAR_TO_CAR, iiffffff);
inline void ATTACH_CAR_TO_CAR(int car, int toCar, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ)
{
    CallScriptCommand(&scm_ATTACH_CAR_TO_CAR, car, toCar, offsetX, offsetY, offsetZ, rotX, rotY, rotZ);
}

//00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
static DEFOPCODE(00A5, CREATE_CAR_AT, ifffv);
inline int CREATE_CAR_AT(int modelId, float x, float y, float z)
{
    int car = 0;
    CallScriptCommand(&scm_CREATE_CAR_AT, modelId, x, y, z, &car);
    return car;
}

//01C8: $P2 = create_actor_pedtype 23 model 280 in_car $POLICE_CAR passenger_seat 0 
static DEFOPCODE(01C8, CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, iiiiv);
inline int CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(int vehicle, PedType pedType, int modelId, int seatId)
{
    int ped = 0;
    CallScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT, vehicle, (int)pedType, modelId, seatId, &ped);
    return ped;
}

//02D4: car 3@ turn_off_engine 
static DEFOPCODE(02D4, CAR_TURN_OFF_ENGINE, i);
inline void CAR_TURN_OFF_ENGINE(int car)
{
    CallScriptCommand(&scm_CAR_TURN_OFF_ENGINE, car);
}

//02C0: store_to 3@ 4@ 5@ ped_path_coords_closest_to 0@ 1@ 2@
static DEFOPCODE(02C0, STORE_PED_PATH_COORDS_CLOSEST_TO, fffvvv);
inline CVector STORE_PED_PATH_COORDS_CLOSEST_TO(float x, float y, float z)
{
    CVector result = CVector(0, 0, 0);
    CallScriptCommand(&scm_STORE_PED_PATH_COORDS_CLOSEST_TO, x, y, z, &result.x, &result.y, &result.z);
    return result;
}

static DEFOPCODE(0129, CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, iiiv); //0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
inline int CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(int car, PedType pedType, int modelId)
{
    int _char = 0;
    CallScriptCommand(&scm_CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT, car, (int)pedType, modelId, &_char);
    return _char;
}

//009B: destroy_actor 4@ 
static DEFOPCODE(009B, DESTROY_ACTOR, i);
inline void DESTROY_ACTOR(int actor)
{
    CallScriptCommand(&scm_DESTROY_ACTOR, actor);
}

//00A6: destroy_car 7@
static DEFOPCODE(00A6, DESTROY_CAR, i);
inline void DESTROY_CAR(int car)
{
    sautils->ScriptCommand(&scm_DESTROY_CAR, car);
}

//0186: $60 = create_marker_above_car $59 
static DEFOPCODE(0186, ADD_BLIP_FOR_CAR, iv);
inline int ADD_BLIP_FOR_CAR(int car)
{
    int blip = 0;
    CallScriptCommand(&scm_ADD_BLIP_FOR_CAR, car, &blip);
    return blip;
}

//0165: set_marker 9@ color_to 2
static DEFOPCODE(0165, SET_MARKER_COLOR_TO, ii);
inline void SET_MARKER_COLOR_TO(int blip, int color)
{
    sautils->ScriptCommand(&scm_SET_MARKER_COLOR_TO, blip, color);
}

// 044B: actor %1d male
static DEFOPCODE(044B, ACTOR_MALE, i);
inline bool ACTOR_MALE(int actor)
{
    return CallScriptCommand_NoLog(&scm_ACTOR_MALE, actor);
}

// 044C: actor %1d driving
static DEFOPCODE(044C, ACTOR_DRIVING, i);
inline bool ACTOR_DRIVING(int actor)
{
    return CallScriptCommand_NoLog(&scm_ACTOR_DRIVING, actor);
}

//01B2: give_actor 3@ weapon 22 ammo 10000
static DEFOPCODE(01B2, GIVE_ACTOR_WEAPON, iii);
inline void GIVE_ACTOR_WEAPON(int _char, int weaponType, int ammo)
{
    CallScriptCommand(&scm_GIVE_ACTOR_WEAPON, _char, weaponType, ammo);
}

// 02E3: car %1d speed
static DEFOPCODE(02E3, CAR_SPEED, iv);
inline float CAR_SPEED(int vehicle)
{
    float speed = 0.0f;
    CallScriptCommand_NoLog(&scm_CAR_SPEED, vehicle, &speed);
    return speed;
}

// 0227: 5@ = car 23@ health 
static DEFOPCODE(0227, GET_CAR_HEALTH, iv);
inline int GET_CAR_HEALTH(int vehicle)
{
    int result = 0;
    CallScriptCommand_NoLog(&scm_GET_CAR_HEALTH, vehicle, &result);
    return result;
}

// 0224: set_car $2868 health_to 1000 
static DEFOPCODE(0224, SET_CAR_HEALTH, ii);
inline void SET_CAR_HEALTH(int vehicle, int health)
{
    CallScriptCommand(&scm_SET_CAR_HEALTH, vehicle, health);
}


// 01BD: put_car_at %1d x %2f y %3f z %4f
static DEFOPCODE(01BD, PUT_CAR_AT, ifff);
inline void PUT_CAR_AT(int vehicle, float x, float y, float z)
{
    CallScriptCommand(&scm_PUT_CAR_AT, vehicle, x, y, z);
}

// 0175: set_car_z_angle %1d to %2f
static DEFOPCODE(0175, SET_CAR_Z_ANGLE, if);
inline void SET_CAR_Z_ANGLE(int vehicle, float heading)
{
    CallScriptCommand(&scm_SET_CAR_Z_ANGLE, vehicle, heading);
}

// 017A: set_car_immunities %1d BP %2d FP %3d EP %4d CP %5d MP %6d
static DEFOPCODE(017A, SET_CAR_IMMUNITIES, iiiiii);
inline void SET_CAR_IMMUNITIES(int vehicle, bool BP, bool FP, bool EP, bool CP, bool MP)
{
    CallScriptCommand(&scm_SET_CAR_IMMUNITIES, vehicle, BP ? 1 : 0, FP ? 1 : 0, EP ? 1 : 0, CP ? 1 : 0, MP ? 1 : 0);
}

// 03AC: load_requested_models
static DEFOPCODE(03AC, LOAD_REQUESTED_MODELS, );
inline void LOAD_REQUESTED_MODELS()
{
    CallScriptCommand(&scm_LOAD_REQUESTED_MODELS);
}

// 038B: load_requested_anims
static DEFOPCODE(038B, LOAD_REQUESTED_ANIMS, );
inline void LOAD_REQUESTED_ANIMS()
{
    CallScriptCommand(&scm_LOAD_REQUESTED_ANIMS);
}

// 0247: request_model %1d
static DEFOPCODE(0247, REQUEST_MODEL, i);
inline void REQUEST_MODEL(int modelId)
{
    CallScriptCommand(&scm_REQUEST_MODEL, modelId);
}

// 0249: release_model %1d
static DEFOPCODE(0249, RELEASE_MODEL, i);
inline void RELEASE_MODEL(int modelId)
{
    CallScriptCommand(&scm_RELEASE_MODEL, modelId);
}

//09C7: change_player 0 model_to 280
static DEFOPCODE(09C7, CHANGE_PLAYER_MODEL_TO, ii);
inline void CHANGE_PLAYER_MODEL_TO(int player, int modelId)
{
    CallScriptCommand(&scm_CHANGE_PLAYER_MODEL_TO, player, modelId);
}

//04EE: animation "GANGS" loaded 
static DEFOPCODE(04EE, HAS_ANIMATION_LOADED, s);
inline bool HAS_ANIMATION_LOADED(const char* animationFile)
{
    bool result = CallScriptCommand_NoLog(&scm_HAS_ANIMATION_LOADED, animationFile);
    return result;
}

//04ED: load_animation "GANGS"
static DEFOPCODE(04ED, LOAD_ANIMATION, s);
inline void LOAD_ANIMATION(const char* animationFile)
{
    CallScriptCommand(&scm_LOAD_ANIMATION, animationFile);
}

//0256: player $PLAYER_CHAR defined 
static DEFOPCODE(0256, PLAYER_DEFINED, i);
inline bool PLAYER_DEFINED(int player)
{
    bool result = CallScriptCommand_NoLog(&scm_PLAYER_DEFINED, 0);
    return result;
}

static DEFOPCODE(056E, CAR_DEFINED, i); //056E: car 3@ defined
inline bool CAR_DEFINED(int car)
{
    bool result = sautils->ScriptCommand(&scm_CAR_DEFINED, car);
    return result;
}

static DEFOPCODE(056D, ACTOR_DEFINED, i); //056D:   actor 0@ defined 
inline bool ACTOR_DEFINED(int actor)
{
    bool result = false;
    result = sautils->ScriptCommand(&scm_ACTOR_DEFINED, actor);
    return result;
}

// 0248: model %1d available
static DEFOPCODE(0248, HAS_MODEL_LOADED, i);
inline bool HAS_MODEL_LOADED(int modelId)
{
    return CallScriptCommand_NoLog(&scm_HAS_MODEL_LOADED, modelId);
}

// 02F6: actor %1d in_zone %2s
static DEFOPCODE(02F6, ACTOR_IN_ZONE, is);
inline bool ACTOR_IN_ZONE(int actor, const char* zoneName)
{
    return CallScriptCommand_NoLog(&scm_ACTOR_IN_ZONE, actor, zoneName);
}

// 0107: player %1d money += %2d
static DEFOPCODE(0107, ADD_PLAYER_MONEY, ii);
inline void ADD_PLAYER_MONEY(int player, int amount)
{
    CallScriptCommand(&scm_ADD_PLAYER_MONEY, player, amount);
}

// 0109: player %1d money = %2d
static DEFOPCODE(0109, SET_PLAYER_MONEY, ii);
inline void SET_PLAYER_MONEY(int player, int amount)
{
    CallScriptCommand(&scm_SET_PLAYER_MONEY, player, amount);
}

// 0108: player %1d money
static DEFOPCODE(0108, GET_PLAYER_MONEY, iI);
inline int GET_PLAYER_MONEY(int player)
{
    int money = 0;
    CallScriptCommand(&scm_GET_PLAYER_MONEY, player, &money);
    return money;
}

// 01C2: remove_references_to_actor %1d
static DEFOPCODE(01C2, REMOVE_REFERENCES_TO_ACTOR, i);
inline void REMOVE_REFERENCES_TO_ACTOR(int actor)
{
    CallScriptCommand(&scm_REMOVE_REFERENCES_TO_ACTOR, actor);
}

// 01C3: remove_references_to_car %1d
static DEFOPCODE(01C3, REMOVE_REFERENCES_TO_CAR, i);
inline void REMOVE_REFERENCES_TO_CAR(int vehicle)
{
    CallScriptCommand(&scm_REMOVE_REFERENCES_TO_CAR, vehicle);
}

static DEFOPCODE(01EA, CAR_MAX_PASSENGERS, iv); //01EA: 68@ = car 67@ max_passengers 
inline int CAR_MAX_PASSENGERS(int car)
{
    int maxPassengers = 0;
    CallScriptCommand_NoLog(&scm_CAR_MAX_PASSENGERS, car, &maxPassengers);
    return maxPassengers;
}

//0432: 19@ = get_actor_handle_from_car $47 passenger_seat 0 
static DEFOPCODE(0432, GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, iiv);
inline int GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(int car, int seatId)
{
    int _char = 0;
    CallScriptCommand_NoLog(&scm_GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT, car, seatId, &_char);
    return _char;
}

//0665: get_actor 0@ model_to 7@
static DEFOPCODE(0665, GET_ACTOR_MODEL, iv);
inline int GET_ACTOR_MODEL(int _char)
{
    int modelId = 0;
    CallScriptCommand_NoLog(&scm_GET_ACTOR_MODEL, _char, &modelId);
    return modelId;
}

//0441: 7@ = car $47 model 
static DEFOPCODE(0441, GET_CAR_MODEL, iv);
inline int GET_CAR_MODEL(int car)
{
    int modelId = 0;
    sautils->ScriptCommand(&scm_GET_CAR_MODEL, car, &modelId);
    return modelId;
}

//09C9: disembark_actor $132 from_car 44@ and_freeze_actor_position 
static DEFOPCODE(09C9, REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION, ii);
inline void REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION(int actor, int car)
{
    CallScriptCommand(&scm_REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION, actor, car);
}

//0431: car $47 passenger_seat_free 0
static DEFOPCODE(0431, CAR_PASSENGER_SEAT_FREE, ii);
inline bool CAR_PASSENGER_SEAT_FREE(int car, int seatId)
{
    bool result = false;
    result = CallScriptCommand_NoLog(&scm_CAR_PASSENGER_SEAT_FREE, car, seatId);
    return result;
}

static DEFOPCODE(00A7, CAR_DRIVE_TO, ifff); //00A7: car 7@ drive_to 0@ 1@ 2@
inline void CAR_DRIVE_TO(int car, float x, float y, float z)
{
    CallScriptCommand(&scm_CAR_DRIVE_TO, car, x, y, z);
}

// 01C4: remove_references_to_object %1d
static DEFOPCODE(01C4, REMOVE_REFERENCES_TO_OBJECT, i);
inline void REMOVE_REFERENCES_TO_OBJECT(int obj)
{
    CallScriptCommand(&scm_REMOVE_REFERENCES_TO_OBJECT, obj);
}

// 01C7: remove_references_to_car %1d
static DEFOPCODE(01C7, REMOVE_REFERENCES_TO_HELI, i);
inline void REMOVE_REFERENCES_TO_HELI(int heli)
{
    CallScriptCommand(&scm_REMOVE_REFERENCES_TO_HELI, heli);
}

// 01C8: remove_references_to_pickup %1d
static DEFOPCODE(01C8, REMOVE_REFERENCES_TO_PICKUP, i);
inline void REMOVE_REFERENCES_TO_PICKUP(int pickup)
{
    CallScriptCommand(&scm_REMOVE_REFERENCES_TO_PICKUP, pickup);
}

//04C4: store_coords_to 4@ 5@ 6@ from_actor $PLAYER_ACTOR with_offset 1.0 2.0 0.0 
static DEFOPCODE(04C4, STORE_COORDS_FROM_ACTOR_WITH_OFFSET, ifffvvv);
inline void STORE_COORDS_FROM_ACTOR_WITH_OFFSET(int _char, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    CallScriptCommand(&scm_STORE_COORDS_FROM_ACTOR_WITH_OFFSET, _char, offsetX, offsetY, offsetZ, x, y, z);
}

//00DF: actor $PLAYER_ACTOR driving 
static DEFOPCODE(00DF, IS_CHAR_IN_ANY_CAR, i);
inline bool IS_CHAR_IN_ANY_CAR(int _char)
{
    bool result = CallScriptCommand_NoLog(&scm_IS_CHAR_IN_ANY_CAR, _char);
    return result;
}

//0811: $47 = actor $PLAYER_ACTOR used_car 
static DEFOPCODE(0811, ACTOR_USED_CAR, iv);
inline int ACTOR_USED_CAR(int _char)
{
    int car = 0;
    CallScriptCommand_NoLog(&scm_ACTOR_USED_CAR, _char, &car);
    return car;
}

//0407: store_coords_to 128@ 138@ 148@ from_car 551@ with_offset -0.337 1.566 0.657 
static DEFOPCODE(0407, STORE_COORDS_FROM_CAR_WITH_OFFSET, ifffvvv);
inline void STORE_COORDS_FROM_CAR_WITH_OFFSET(int car, float offsetX, float offsetY, float offsetZ, float* x, float* y, float* z)
{
    CallScriptCommand(&scm_STORE_COORDS_FROM_CAR_WITH_OFFSET, car, offsetX, offsetY, offsetZ, x, y, z);
}

//0812: AS_actor 0@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1
static DEFOPCODE(0812, PERFORM_ANIMATION_AS_ACTOR, issfbbbbi);
inline void PERFORM_ANIMATION_AS_ACTOR(int _char, const char* animationName, const char* animationFile, float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time)
{
    CallScriptCommand(&scm_PERFORM_ANIMATION_AS_ACTOR, _char, animationName, animationFile, frameDelta, loop, lockX, lockY, lockF, time);
}

//0464: put_actor $PED into_turret_on_car $CAR at_car_offset 0.0 0.0 0.0 position 0 shooting_angle_limit 0.0 with_weapon 0
static DEFOPCODE(0464, PUT_ACTOR_INTO_TURRET_ON_CAR, iifffifi);
inline void PUT_ACTOR_INTO_TURRET_ON_CAR(int _char, int vehicle, float offsetX, float offsetY, float offsetZ, int position, float angleLimit, int weaponType)
{
    sautils->ScriptCommand(&scm_PUT_ACTOR_INTO_TURRET_ON_CAR, _char, vehicle, offsetX, offsetY, offsetZ, position, angleLimit, weaponType);
}

//0465: remove_actor $PED from_turret_mode
static DEFOPCODE(0465, REMOVE_ACTOR_FROM_TURRET_MODE, i);
inline void REMOVE_ACTOR_FROM_TURRET_MODE(int _char)
{
    sautils->ScriptCommand(&scm_REMOVE_ACTOR_FROM_TURRET_MODE, _char);
}

//009A: 6@ = create_actor_pedtype 20 model #DNFYLC at 3@ 4@ 5@
static DEFOPCODE(009A, CREATE_ACTOR_PEDTYPE, iifffv);
inline int CREATE_ACTOR_PEDTYPE(PedType pedType, int modelId, float x, float y, float z)
{
    int _char = 0;
    CallScriptCommand(&scm_CREATE_ACTOR_PEDTYPE, (int)pedType, modelId, x, y, z, &_char);
    return _char;
}

//0726: heli 3@ follow_actor -1 follow_car 5@ radius 15.0
static DEFOPCODE(0726, HELI_FOLLOW, iiif);
inline void HELI_FOLLOW(int heli, int _char, int vehicle, float radius)
{
    sautils->ScriptCommand(&scm_HELI_FOLLOW, heli, _char, vehicle, radius);
}

//0743: heli 45@ fly_to -2244.48 129.14 34.56 altitude 0.0 0.0 
static DEFOPCODE(0743, HELI_FLY_TO, ifffff);
inline void HELI_FLY_TO(int heli, float x, float y, float z, float minAltitude, float maxAltitude)
{
    sautils->ScriptCommand(&scm_HELI_FLY_TO, heli, x, y, z, minAltitude, maxAltitude);
}

//0687: clear_actor $PLAYER_ACTOR task
static DEFOPCODE(0687, CLEAR_ACTOR_TASK, i);
inline void CLEAR_ACTOR_TASK(int _char)
{
    CallScriptCommand(&scm_CLEAR_ACTOR_TASK, _char);
}

//0611: actor 2@ performing_animation "LRGIRL_IDLE_TO_L0"
static DEFOPCODE(0611, ACTOR_PERFORMING_ANIMATION, is);
inline bool ACTOR_PERFORMING_ANIMATION(int _char, const char* animationName)
{
    bool result = CallScriptCommand(&scm_ACTOR_PERFORMING_ANIMATION, _char, animationName);
    return result;
}

//0603: AS_actor @3 goto_point_any_means 2493.82 -1669.91 12.8 mode 7 use_car -1 
static DEFOPCODE(0603, TASK_GO_TO_COORD_ANY_MEANS, ifffib);
inline void TASK_GO_TO_COORD_ANY_MEANS(int actor, float x, float y, float z, int mode, bool useCar)
{
    CallScriptCommand(&scm_TASK_GO_TO_COORD_ANY_MEANS, actor, x, y, z, mode, useCar ? 1 : 0);
}

//0918: set_car 3@ engine_operation 1 
static DEFOPCODE(0918, SET_CAR_ENGINE_OPERATION, ib);
inline void SET_CAR_ENGINE_OPERATION(int car, bool state)
{
    CallScriptCommand(&scm_SET_CAR_ENGINE_OPERATION, car, state);
}

//0825: set_helicopter 3@ instant_rotor_start
static DEFOPCODE(0825, SET_HELICOPTER_INSTANT_ROTOR_START, i);
inline void SET_HELICOPTER_INSTANT_ROTOR_START(int heli)
{
    sautils->ScriptCommand(&scm_SET_HELICOPTER_INSTANT_ROTOR_START, heli);
}

enum DrivingMode
{
   StopForCars = 0,
   SlowDownForCars = 1,
   AvoidCars = 2,
   PloughThrough = 3,
   StopForCarsIgnoreLights = 4,
   AvoidCarsObeyLights = 5,
   AvoidCarsStopForPedsObeyLights = 6
};

//00AE: set_car 3@ traffic_behaviour_to 2
static DEFOPCODE(00AE, SET_CAR_TRAFFIC_BEHAVIOUR, ii);
inline void SET_CAR_TRAFFIC_BEHAVIOUR(int car, DrivingMode drivingStyle)
{
    CallScriptCommand(&scm_SET_CAR_TRAFFIC_BEHAVIOUR, car, (int)drivingStyle);
}

//0397: enable_car 6@ siren 1
static DEFOPCODE(0397, ENABLE_CAR_SIREN, ib);
inline void ENABLE_CAR_SIREN(int car, bool state)
{
    sautils->ScriptCommand(&scm_ENABLE_CAR_SIREN, car, state);
}

//0635: AS_actor -1 aim_at_actor $PLAYER_ACTOR 2000 ms 
static DEFOPCODE(0635, AIM_AT_ACTOR, iii);
inline void AIM_AT_ACTOR(int _char, int target, int time)
{
    sautils->ScriptCommand(&scm_AIM_AT_ACTOR, _char, target, time);
}

//05DD: AS_actor 7@ flee_from_actor 6@ from_origin_radius 1000.0 timelimit -1
static DEFOPCODE(05DD, FLEE_FROM_ACTOR, iifi);
inline void FLEE_FROM_ACTOR(int _char, int threat, float radius, int time)
{
    sautils->ScriptCommand(&scm_FLEE_FROM_ACTOR, _char, threat, radius, time);
}

//00A8: set_car 52@ to_psycho_driver 
static DEFOPCODE(00A8, SET_CAR_TO_PSYCHO_DRIVER, i);
inline void SET_CAR_TO_PSYCHO_DRIVER(int car)
{
    CallScriptCommand(&scm_SET_CAR_TO_PSYCHO_DRIVER, car);
}

//00AD: set_car 3@ max_speed_to 50.0
static DEFOPCODE(00AD, SET_CAR_MAX_SPEED, if);
inline void SET_CAR_MAX_SPEED(int car, float maxSpeed)
{
    CallScriptCommand(&scm_SET_CAR_MAX_SPEED, car, maxSpeed);
}

//07F8: car 6@ follow_car 8@ radius 8.0
static DEFOPCODE(07F8, CAR_FOLLOW_CAR, iif);
inline void CAR_FOLLOW_CAR(int car, int followCar, float radius)
{
    sautils->ScriptCommand(&scm_CAR_FOLLOW_CAR, car, followCar, radius);
}

//046C: 133@ = car 50@ driver 
static DEFOPCODE(046C, GET_DRIVER_OF_CAR, iv);
inline int GET_DRIVER_OF_CAR(int car)
{
    int _char = 0;
    CallScriptCommand_NoLog(&scm_GET_DRIVER_OF_CAR, car, &_char);
    return _char;
}

//03BC: 7@ = create_sphere_at 1536.1325 -1671.2093 13.3828 radius 3.0 
static DEFOPCODE(03BC, CREATE_SPHERE, ffffv);
inline int CREATE_SPHERE(float x, float y, float z, float radius)
{
    int sphere = 0;
    sautils->ScriptCommand(&scm_CREATE_SPHERE, x, y, z, radius, &sphere);
    return sphere;
}

//03BD: destroy_sphere 7@
static DEFOPCODE(03BD, DESTROY_SPHERE, i);
inline void DESTROY_SPHERE(int sphere)
{
    sautils->ScriptCommand(&scm_DESTROY_SPHERE, sphere);
}

//0633: AS_actor 4@ exit_car
static DEFOPCODE(0633, EXIT_CAR_AS_ACTOR, i);
inline void EXIT_CAR_AS_ACTOR(int _actor)
{
    CallScriptCommand(&scm_EXIT_CAR_AS_ACTOR, _actor);
}

//05CB: AS_actor 21@ enter_car 0@ as_driver 20000 ms
static DEFOPCODE(05CB, ENTER_CAR_AS_DRIVER_AS_ACTOR, iii);
inline void ENTER_CAR_AS_DRIVER_AS_ACTOR(int _char, int vehicle, int time)
{
    CallScriptCommand(&scm_ENTER_CAR_AS_DRIVER_AS_ACTOR, _char, vehicle, time);
}

//05CA: AS_actor 3@ enter_car 7@ passenger_seat 1 time 10000
static DEFOPCODE(05CA, ACTOR_ENTER_CAR_PASSENGER_SEAT, iiii);
inline void ACTOR_ENTER_CAR_PASSENGER_SEAT(int _char, int vehicle, int time, int seatId)
{
    CallScriptCommand(&scm_ACTOR_ENTER_CAR_PASSENGER_SEAT, _char, vehicle, time, seatId);
}

//0187: 47@ = create_marker_above_actor 75@ 
static DEFOPCODE(0187, ADD_BLIP_FOR_CHAR, iv);
inline int ADD_BLIP_FOR_CHAR(int _char)
{
    int blip = 0;
    CallScriptCommand(&scm_ADD_BLIP_FOR_CHAR, _char, &blip);
    return blip;
}

//0164: disable_marker $482
static DEFOPCODE(0164, DISABLE_MARKER, i);
inline void DISABLE_MARKER(int blip)
{
    CallScriptCommand(&scm_DISABLE_MARKER, blip);
}

//05E2: AS_actor 6@ kill_actor 7@ 
static DEFOPCODE(05E2, KILL_ACTOR, ii);
inline void KILL_ACTOR(int killer, int target)
{
    sautils->ScriptCommand(&scm_KILL_ACTOR, killer, target);
}

//0226: $7826 = actor 173@ health
static DEFOPCODE(0226, ACTOR_HEALTH, iv);
inline int ACTOR_HEALTH(int _char)
{
    int health = 0;
    sautils->ScriptCommand(&scm_ACTOR_HEALTH, _char, &health);
    return health;
}


// --------------------------------------------

inline void WAIT(int time, std::function<void()> callback)
{
    CleoFunctions::AddWaitFunction(time, callback);
}

// --------------------------------------------

inline int GetPlayerActor()
{
    return GET_PLAYER_ACTOR(0);
}

inline CVector GetPedPositionWithOffset(int hPed, CVector offset)
{
    float x = 0, y = 0, z = 0;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hPed, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

inline CVector GetPedPosition(int hPed)
{
    return GetPedPositionWithOffset(hPed, CVector(0, 0, 0));
}

inline int GetVehiclePedIsUsing(int hPed)
{
    if(!IS_CHAR_IN_ANY_CAR(hPed)) return 0;

    return ACTOR_USED_CAR(hPed);
}

inline void ClearPedAnimations(int hPed)
{
    REMOVE_REFERENCES_TO_ACTOR(hPed);

    if(IS_CHAR_IN_ANY_CAR(hPed)) return;

    PERFORM_ANIMATION_AS_ACTOR(hPed, "hndshkfa_swt", "gangs", 500.0f, 0, 0, 0, 0, 1);
}

inline CVector GetPlayerPosition()
{
    return GetPedPosition(GetPlayerActor());
}

inline CVector GetPlayerPositionInForward(float distanceY)
{
    return GetPedPositionWithOffset(GetPlayerActor(), CVector(0, distanceY, 0));
}

inline CVector GetCarPositionWithOffset(int hVehicle, CVector offset)
{
    float x = 0, y = 0, z = 0;
    STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

inline CVector GetCarPosition(int hVehicle)
{
    return GetCarPositionWithOffset(hVehicle, CVector(0, 0, 0));
}

inline double DistanceFromPed(int hPed, CVector position)
{
    auto pedPosition = GetPedPosition(hPed);
    auto distance = distanceBetweenPoints(pedPosition, position);
    return distance;
}

inline double DistanceFromVehicle(int hCar, CVector position)
{
    auto carPosition = GetCarPosition(hCar);
    auto distance = distanceBetweenPoints(carPosition, position);
    return distance;
}