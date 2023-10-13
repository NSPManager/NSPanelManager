(define (nspanel-eu)
	
	(let* ((layerList (vector->list (cadr (gimp-image-get-layers 1))))) 

	;Start with making all layers visible

	(while (not (null? layerList))
			(gimp-item-set-visible (car layerList) 1)
			(set! layerList (cdr layerList))
		)
	)
	;Make nice-to-have text layer invisble
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "ALL_PAGES_TEXT")) 0)
	
	;Make all "_PAGE" group layers invisible but the first one. When we are finished with saving all the pictures on the FIRST_PAGE we make that invisible and the next one visible and so on...

;;------------------------------------------------------------------------------------------------------------------------	
;FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE FIRST_PAGE 

		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "ROOM_PAGE")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "LIGHTS_PAGE")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "SCENES_PAGE")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for ON export
		
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_off")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)
	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/58_first_page_ON.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/58_first_page_ON.bmp")
		)
	(gimp-image-delete newImage)
	)
		;;prepare for OFF export
		
	(let* 
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_on")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_off")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/57_first_page_OFF.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/57_first_page_OFF.bmp")
		)
	(gimp-image-delete newImage)
	)
		;;prepare for upper left corner crop export
		
	(let* 
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 112 49 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)


	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/59_first_page_scenes_room_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/59_first_page_scenes_room_off.bmp")
		)
	(gimp-image-delete newImage)
	)


	(let* 
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 112 49 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/60_first_page_scenes_room_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/60_first_page_scenes_room_on.bmp")
		)
	(gimp-image-delete newImage)
	)



	(let* 
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 112 49 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 1)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/62_first_page_scenes_all_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/62_first_page_scenes_all_on.bmp")
		)
	(gimp-image-delete newImage)
	)


	(let* 
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 112 49 0 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_all")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_scenes_room")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "first_page_buttons_press")) 0)

	(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/61_first_page_scenes_all_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/61_first_page_scenes_all_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;MAKE FIRST PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "FIRST_PAGE")) 0)

;;------------------------------------------------------------------------------------------------------------------------	
;; ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE ROOM_PAGE 	
	
	;MAKE ROOM PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "ROOM_PAGE")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for empty buttons
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_off")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_on")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/63_room_page_empty.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/63_room_page_empty.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for button press export
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_off")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_on")) 1)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_buttons_press")) 1)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/65_room_page_press.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/65_room_page_press.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for button depress export
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_buttons_press")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_on")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "room_page_toggle_off")) 1)

		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/64_room_page_depress.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/64_room_page_depress.bmp")
		)
	(gimp-image-delete newImage)
	)



	;MAKE LIGHTS PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "ROOM_PAGE")) 0)
	
;;------------------------------------------------------------------------------------------------------------------------	
;; LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE LIGHTS_PAGE 	
	
	;MAKE LIGHTS PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "LIGHTS_PAGE")) 1)
		;make all icon layers invisible before starting
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_buttons_press")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_brightness")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_kelvin")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_kelvin")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_saturation")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_gradient")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_color")) 0)
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_kelvin")) 0)
		
		;;(gimp-image-crop image new-width new-height offx offy)
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for basic light page, no icons
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/66_lights_page_empty.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/66_lights_page_empty.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_brightness")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_kelvin")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_color")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_gradient")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for brightness slider
		(gimp-image-crop newImage 142 260 7 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/67_lights_page_brightness_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/67_lights_page_brightness_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 142 260 155 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/69_lights_page_kelvin_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/69_lights_page_kelvin_off.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for color slider
		(gimp-image-crop newImage 142 260 303 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/71_lights_page_color_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/71_lights_page_color_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;;SAVE BRIGHTNESS KELVIN COLOR WITH PRESS BACKGROUND
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_buttons_press")) 1)
	
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for brightness slider
		(gimp-image-crop newImage 142 260 7 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/68_lights_page_brightness_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/68_lights_page_brightness_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 142 260 155 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/70_lights_page_kelvin_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/70_lights_page_kelvin_on.bmp")
		)
	(gimp-image-delete newImage)
	)

	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for kelvin slider
		(gimp-image-crop newImage 142 260 303 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/72_lights_page_color_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/72_lights_page_color_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;; SAVE SATURATION
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_kelvin")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_saturation")) 1)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_buttons_press")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for saturation slider
		(gimp-image-crop newImage 142 260 155 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/73_lights_page_saturation_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/73_lights_page_saturation_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		;;prepare for saturation slider
		(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "lights_page_buttons_press")) 1)
		(gimp-image-crop newImage 142 260 155 55)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/74_lights_page_saturation_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/74_lights_page_saturation_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	;;SAVE CORNER ICON COLOR KELVIN EMPTY
	
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 70 50 385 0)
		;(gimp-item-set-visible (car (gimp-image-get-layer-by-name newImage "lights_page_buttons_press")) 0)
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/76_lights_page_corner_empty.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/76_lights_page_corner_empty.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_kelvin")) 1)
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-crop newImage 70 50 385 0)

		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/77_lights_page_corner_kelvin.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/77_lights_page_corner_kelvin.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_kelvin")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "lights_page_corner_color")) 1)
	(let*
	
	((newImage (car (gimp-image-duplicate 1))))

		(gimp-image-crop newImage 70 50 385 0)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/78_lights_page_corner_color.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/78_lights_page_corner_color.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	
	;MAKE LIGHTS PAGE INVISIBLE TO PREPARE FOR NEXT ROOM
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "LIGHTS_PAGE")) 0)
	
;;------------------------------------------------------------------------------------------------------------------------	
;; SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE SCENES_PAGE  	
	
	;MAKE LIGHTS PAGE VISIBLE
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "SCENES_PAGE")) 1)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_loadbar_100")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_loadbar_0")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_buttons_press")) 0)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/81_scenes_page_off.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/81_scenes_page_off.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_buttons_press")) 1)
	
	(let*
	((newImage (car (gimp-image-duplicate 1))))
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/82_scenes_page_on.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/82_scenes_page_on.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_buttons_press")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_loadbar_0")) 1)
	
	(let*
	
	((newImage (car (gimp-image-duplicate 1))))

		(gimp-image-crop newImage 236 3 134 43)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/79_scenes_page_loadbar_0.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/79_scenes_page_loadbar_0.bmp")
		)
	(gimp-image-delete newImage)
	)
	
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_loadbar_0")) 0)
	(gimp-item-set-visible (car (gimp-image-get-layer-by-name 1 "scenes_page_loadbar_100")) 1)
	
	(let*
	
	((newImage (car (gimp-image-duplicate 1))))

		(gimp-image-crop newImage 236 3 134 43)
		
		(gimp-image-merge-visible-layers newImage 1)
		(let*
		((activeLayer (car (gimp-image-get-active-layer newImage))))
		(gimp-file-save 1 1 activeLayer "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/80_scenes_page_loadbar_100.bmp" "/home/erik/Documents/Projekt/NSPanel Manager/Design/automation/eu/80_scenes_page_loadbar_100.bmp")
		)
	(gimp-image-delete newImage)
	)
	
)

(script-fu-register "nspanel-eu"
  _"To _Image"
  _"Convert a selection to an image"
  "Adrian Likins <adrian@gimp.org>"
  "Adrian Likins"
  "10/07/97"
  "RGB* GRAY*"
)
