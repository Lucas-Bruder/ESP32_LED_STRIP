idf_component_register(
     SRCS 
         "components/led_strip/led_strip.c" 
     INCLUDE_DIRS
         "components/led_strip/inc" 
     PRIV_REQUIRES
         "nvs_flash"
)
 