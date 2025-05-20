#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "core/kmemory.h"
#include "game_types.h"

// External-defined function to create a game
extern b8 create_game(game* game_inst);

/**
 * The main enry point of the application.
 */

 int main(void)
    {
        initialize_memory();
        // Request the game instance from the application.
        game game_inst = {0};
    
    
        if(!create_game(&game_inst))
        {
            KERROR("Could not create game! ");
            return -1;
        }
    
        // Ensure the function pointers exist.
        if(!game_inst.render || !game_inst.update ||!game_inst.initialize || !game_inst.on_resize)
        {
            KFATAL("The game's function pointers must be assigned.");
            return -2;
        }
        // Initialization.
        if(!applicatio_create(&game_inst))
        {
            KFATAL("Application failed to create.");
            return 1;
        }

        // Begin the game loop.
        if(!application_run())
        {
            KINFO("Application did not shutdown gracefully.");
            return 2;
        }
        
        shutdown_memory();
        
        return 0;
    }