#include "application.h"
#include "game_types.h"

#include "logger.h"

#include "platform/platform.h"
#include "core/kmemory.h"
#include "core/event.h"
#include "core/input.h"

typedef struct application_state
{
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;

static b8 initialized = false;
static application_state app_state;

b8 applicatio_create(struct game* game_inst)
{
    if(initialized)
    {
        KERROR("application_create called more than once");
        return false;
    }

    app_state.game_inst = game_inst;

    // Initialize subsystems.
    initialize_logging();
    input_initialize();

    // TODO: Remove this
    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f", 3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message: %f", 3.14f);
    KTRACE("A test message: %f", 3.14f);

    app_state.is_running = true;
    app_state.is_suspended = false;

    if(!event_initialize())
    {
        KERROR("Event system failed initialization. Application cannot continue.");
        return false;
    }

    if(!platform_startup(
        &app_state.platform,
        game_inst->app_config.name,
        game_inst->app_config.start_pos_x,
        game_inst->app_config.start_pos_y,
        game_inst->app_config.start_width,
        game_inst->app_config.start_height))
        {
            return false;
        }
    
    // Initialize the game.
    if(!app_state.game_inst->initialize(app_state.game_inst))
    {
        KFATAL("Failed to initialize.");
        return false;
    }

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);
    
    initialized = true;
    return true;
}

b8 application_run()
{
    KINFO(get_memory_usage_str());
    
    while (app_state.is_running)
    {
        if(!platform_pump_messages(&app_state.platform))
        {
            app_state.is_running = false;
        }
        if(!app_state.is_suspended)
        {
            if(!app_state.game_inst->update(app_state.game_inst, (f32)0))
            {
                KFATAL("Game update failed, shutting down.");
                app_state.is_running = false;
                break;
            }
            
        }
        
        // Call the game's render routine.
        if(!app_state.game_inst->render(app_state.game_inst, (f32)0))
        {
            KFATAL("Game render failed, shutting down.");
            app_state.is_running = false;
            break;
        }
        
        // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
        input_update(0);
    } 
       
    app_state.is_running = false;

    event_shutdown();
    input_shutdown();

    platform_shutdown(&app_state.platform);
    return true;
}

