idf_component_register(
    SRCS "indicators.c" "artnet.c" "main.c" "settings.c" "output.c" "indicators.c" "rdm.c" "http.c"
    INCLUDE_DIRS "."
)

spiffs_create_partition_image(web_content webpages FLASH_IN_PROJECT)
