set(srcs 
"fileDispatch.c"
"font9x16.c"
"LCD.c"
"main_menu.c"
"sd_card.c"
"smain.c"
"tape.c"
"tv_out.c"
"usb_host.c"
"z80.c"
    )


idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." )

