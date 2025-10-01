#pragma once

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include <iostream>
#include <map>       
#include <string>  
#include <vector>
#include <filesystem>
#include <math.h>
#include <sys/stat.h>
#include <sstream>
#include <random>
#include <cstdlib>
#include <ctime>

#include "simpleGta.h"
#include "Log.h"
#include "Globals.h"
#include "utils.h"

#include "json/json.h"

#define NO_PED_FOUND -1
#define NO_BLIP 0

#define CHASE_MAX_VEHICLE_SPEED 30.0f

#define debug menuSZK->GetDebug()