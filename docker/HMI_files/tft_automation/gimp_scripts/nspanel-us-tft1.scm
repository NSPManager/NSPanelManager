(define (nspanel-us-tft1 xcf-path)
  (let* (
         (image (car (gimp-file-load RUN-NONINTERACTIVE xcf-path xcf-path)))
         ;(drawable (car (gimp-image-get-active-layer image)))
         ;(layerList (vector->list (cadr (gimp-image-get-layers image))))
        )
        
	(let* ((layerList (vector->list (cadr (gimp-image-get-layers image))))) 

	;Start with making all layers visible

	(while (not (null? layerList))
			(gimp-item-set-visible (car layerList) 1)
			(set! layerList (cdr layerList))
		)
	)
	;Make nice-to-have text layer invisble
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ALL_PAGES_TEXT")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "TESTLAYER")) 0)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "tft1")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "tft2")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "tft3")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "tft4")) 0)
	
	;Make all "_PAGE" group layers invisible but the first one. When we are finished with saving all the pictures on the FIRST_PAGE we make that invisible and the next one visible and so on...

;;------------------------------------------------------------------------------------------------------------------------	
;FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE 

		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_8")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_12")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "LIGHTS_PAGE")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "SCREENSAVER_PAGE")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "DROPDOWN_PAGE")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "FIRST_PAGE_SOUND")) 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_BLINDS")) 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_THERMOSTAT")) 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_TV")) 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ALARM PAGE")) 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "SCREENCLEAN_PAGE")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for ON export
		
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_off")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)
	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/58_first_page_ON.bmp" "pictures_tft1/58_first_page_ON.bmp")
		)
	(gimp-image-delete newImage)
	)
		;;prepare for OFF export
		
	(let* 
	((newImage (car (gimp-image-duplicate image))))
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_on")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_off")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/57_first_page_OFF.bmp" "pictures_tft1/57_first_page_OFF.bmp")
		)
	(gimp-image-delete newImage)
	)
		;;prepare for upper left corner crop export
		
	(let* 
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 81 52 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)


	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/59_first_page_scenes_room_off.bmp" "pictures_tft1/59_first_page_scenes_room_off.bmp")
		)
	(gimp-image-delete newImage)
	)


	(let* 
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 81 52 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/60_first_page_scenes_room_on.bmp" "pictures_tft1/60_first_page_scenes_room_on.bmp")
		)
	(gimp-image-delete newImage)
	)



	(let* 
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 81 52 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/62_first_page_scenes_all_on.bmp" "pictures_tft1/62_first_page_scenes_all_on.bmp")
		)
	(gimp-image-delete newImage)
	)


	(let* 
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 81 52 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/61_first_page_scenes_all_off.bmp" "pictures_tft1/61_first_page_scenes_all_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;MAKE FIRST PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "FIRST_PAGE")) 0)

;;------------------------------------------------------------------------------------------------------------------------	
;; ENTITY_PAGE_4 ENTITY_PAGE_4 ENTITY_PAGE_4 ENTITY_PAGE_4 ENTITY_PAGE_4 ENTITY_PAGE_4 ENTITY_PAGE_4 
	
	;MAKE ROOM PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_buttons_depress")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_loadbar_100")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_loadbar_0")) 0)
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for empty buttons
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_4_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/67_ENTITY_PAGE_4_empty.bmp" "pictures_tft1/67_ENTITY_PAGE_4_empty.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for button press export
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_4_buttons_press")) 1)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/68_ENTITY_PAGE_4_press.bmp" "pictures_tft1/68_ENTITY_PAGE_4_press.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_loadbar_0")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_buttons_depress")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_buttons_press")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))

		(gimp-image-crop newImage 184 3 93 46)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/73_scenes_page_loadbar_0.bmp" "pictures_tft1/73_scenes_page_loadbar_0.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_loadbar_0")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4_loadbar_100")) 1)
	
	(let*
	
	((newImage (car (gimp-image-duplicate image))))

		(gimp-image-crop newImage 184 3 93 46)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/74_scenes_page_loadbar_100.bmp" "pictures_tft1/74_scenes_page_loadbar_100.bmp")
		)
	(gimp-image-delete newImage)
	)

	;MAKE LIGHTS PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_4")) 0)

;;------------------------------------------------------------------------------------------------------------------------	
;; ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8 ENTITY_PAGE_8  	
	
	;MAKE ROOM PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_8")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for empty buttons
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_8_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/69_ENTITY_PAGE_8_empty.bmp" "pictures_tft1/69_ENTITY_PAGE_8_empty.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for button press export
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_8_buttons_press")) 1)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/70_ENTITY_PAGE_8_press.bmp" "pictures_tft1/70_ENTITY_PAGE_8_press.bmp")
		)
	(gimp-image-delete newImage)
	)


	;MAKE PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_8")) 0)




;;------------------------------------------------------------------------------------------------------------------------	
;; ENTITY_PAGE_12 ENTITY_PAGE_12 ENTITY_PAGE_12 ENTITY_PAGE_12 ENTITY_PAGE_12 ENTITY_PAGE_12 ENTITY_PAGE_12 	
	
	;MAKE ROOM PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_12" )) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for empty buttons
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_12_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/71_ENTITY_PAGE_12_empty.bmp" "pictures_tft1/71_ENTITY_PAGE_12_empty.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for button press export
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "ENTITY_PAGE_12_buttons_press")) 1)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/72_ENTITY_PAGE_12_press.bmp" "pictures_tft1/72_ENTITY_PAGE_12_press.bmp")
		)
	(gimp-image-delete newImage)
	)



	;MAKE LIGHTS PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "ENTITY_PAGE_12" )) 0)
;;------------------------------------------------------------------------------------------------------------------------	
;; LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE 	
	
	;MAKE LIGHTS PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "LIGHTS_PAGE")) 1)
		;make all icon layers invisible before starting
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_buttons_press")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_brightness")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_kelvin")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_kelvin")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_saturation")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_gradient")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_kelvin")) 0)
		
		;;(gimp-image-crop image new-width new-height offx offy)
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for basic light page, no icons
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/75_lights_page_empty.bmp" "pictures_tft1/75_lights_page_empty.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_brightness")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_kelvin")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_color")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_gradient")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for brightness slider
		(gimp-image-crop newImage 307 134 7 58)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/76_lights_page_brightness_off.bmp" "pictures_tft1/76_lights_page_brightness_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 307 135 7 198)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/78_lights_page_kelvin_off.bmp" "pictures_tft1/78_lights_page_kelvin_off.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for color slider
		(gimp-image-crop newImage 307 135 7 339)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/80_lights_page_color_off.bmp" "pictures_tft1/80_lights_page_color_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;;SAVE BRIGHTNESS KELVIN COLOR WITH PRESS BACKGROUND
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_buttons_press")) 1)
	
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for brightness slider
		(gimp-image-crop newImage 307 134 7 58)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/77_lights_page_brightness_on.bmp" "pictures_tft1/77_lights_page_brightness_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 307 135 7 198)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/79_lights_page_kelvin_on.bmp" "pictures_tft1/79_lights_page_kelvin_on.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 307 135 7 339)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/81_lights_page_color_on.bmp" "pictures_tft1/81_lights_page_color_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;; SAVE SATURATION
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_kelvin")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_saturation")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_buttons_press")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for saturation slider
		(gimp-image-crop newImage 307 135 7 198)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/82_lights_page_saturation_off.bmp" "pictures_tft1/82_lights_page_saturation_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		;;prepare for saturation slider
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "lights_page_buttons_press")) 1)
		(gimp-image-crop newImage 307 135 7 198)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/83_lights_page_saturation_on.bmp" "pictures_tft1/83_lights_page_saturation_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;;SAVE CORNER ICON COLOR KELVIN EMPTY
	
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 60 55 260 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "lights_page_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/85_lights_page_corner_empty.bmp" "pictures_tft1/85_lights_page_corner_empty.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_kelvin")) 1)
	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-crop newImage 60 55 260 0)

		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/86_lights_page_corner_kelvin.bmp" "pictures_tft1/86_lights_page_corner_kelvin.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_kelvin")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "lights_page_corner_color")) 1)
	(let*
	
	((newImage (car (gimp-image-duplicate image))))

		(gimp-image-crop newImage 60 55 260 0)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/87_lights_page_corner_color.bmp" "pictures_tft1/87_lights_page_corner_color.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	
	;MAKE LIGHTS PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "LIGHTS_PAGE")) 0)
	
;;------------------------------------------------------------------------------------------------------------------------	
;; SCREENSAVER_PAGE SCREENSAVER_PAGE SCREENSAVER_PAGE SCREENSAVER_PAGE SCREENSAVER_PAGE SCREENSAVER_PAGE SCREENSAVER_PAGE 
	
	;MAKE BACKGROUND PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "SCREENSAVER_PAGE")) 1)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/63_screensaver_page.bmp" "pictures_tft1/63_screensaver_page.bmp")
		)
	(gimp-image-delete newImage)
	)


	;MAKE BACKGROUND PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "SCREENSAVER_PAGE")) 0)
	

;;------------------------------------------------------------------------------------------------------------------------	
;; DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE DROPDOWN_PAGE    	
	
	;MAKE BACKGROUND PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "DROPDOWN_PAGE")) 1)
	;Prepare depress picture
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "dropdown_page_arrows")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "dropdown_page_buttons_depress")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "dropdown_page_buttons_press")) 0)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/64_dropdown_depress.bmp" "pictures_tft1/64_dropdown_depress.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;prepare press picture
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "dropdown_page_buttons_press")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/65_dropdown_press.bmp" "pictures_tft1/65_dropdown_press.bmp")
		)
	(gimp-image-delete newImage)
	)


	;MAKE BACKGROUND PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "DROPDOWN_PAGE")) 0)

;;------------------------------------------------------------------------------------------------------------------------	
;; BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE   	
	
	;MAKE BACKGROUND PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "BACKGROUND")) 1)

	(let*
	((newImage (car (gimp-image-duplicate image))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save RUN-NONINTERACTIVE image activeLayer "pictures_tft1/66_background.bmp" "pictures_tft1/66_background.bmp")
		)
	(gimp-image-delete newImage)
	)


	;MAKE BACKGROUND PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	;(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "BACKGROUND")) 0)
	
	(gimp-image-delete image)
	)
)


(script-fu-register "nspanel-us-tft1"
  _"Export NSPanel US"
  _"Export all UI states to BMP from XCF"
  "Erik Bergström"
  "GPL"
  "2025"
  ""
  SF-STRING "XCF Path" "")
  
  
  ;; NEW ROOM EXAMPLE
;;-----------------------------------------------------------------------------------------------------------------------	
;; BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE BACKGROUND_PAGE   	
;;	
;;	;MAKE BACKGROUND PAGE VISIBLE
;;	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "BACKGROUND_PAGE")) 1)
;;
;;	;MAKE BACKGROUND PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
;;	(gimp-item-set-visible (car (gimp-image-get-layer-by-name image "BACKGROUND_PAGE")) 0)	
