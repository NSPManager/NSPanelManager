#Requires AutoHotkey v2.0
#SingleInstance Force
CoordMode("Mouse", "Window")

; ── CONFIG ─────────────────────────────
hmiFile        := "C:\tft_automation\us\gui.HMI"
inputImageDir1 := "C:\tft_automation\us\pictures_tft1"
inputImageDir2 := "C:\tft_automation\us\pictures_tft2"
inputImageDir3 := "C:\tft_automation\us\pictures_tft3"
inputImageDir4 := "C:\tft_automation\us\pictures_tft4"
outputTftDir1  := "C:\tft_automation\us\output_tft1"
outputTftDir2  := "C:\tft_automation\us\output_tft2"
outputTftDir3  := "C:\tft_automation\us\output_tft3"
outputTftDir4  := "C:\tft_automation\us\output_tft4"

; ── STEP 1: Open HMI file (starts editor too) ─
Run(hmiFile)
WinWaitActive("Nextion Editor") ; Wait for the window title to show up
Sleep(2000) ; extra buffer to let it finish loading fully

; ── STEP 2: Delete all images ────────────────
Click(195, 444)        ; Trash icon
Sleep(500)
Send("{Enter}")        ; Confirm delete
Sleep(1000)

; ── STEP 3: Add new pictures ─────────────────
Click(27, 444)         ; Plus icon (Add)
;WinWaitActive("Add Pictures")

WinWaitActive("ahk_class #32770")      ; the file-chooser window

Send("!d")              ; focus the path field
Sleep(100)
SendText(inputImageDir1)
Send("{Enter}")         ; navigate
Sleep(800)                                   ; let the folder refresh

;Shift focus into the file list
Send("{Tab 4}")                             ; 4x Tab to reach the file list
Sleep(100)
Send("^a")                                  ; Select all files
Sleep(100)
Send("{Enter}")                             ; Import into Nextion

WinWaitClose("ahk_class #32770", , 5)
Sleep(5000)
; wait for the success pop-up and dismiss it
;WinWaitActive("Nextion Editor", "Import successful", 5)  ; title + text filter
;Sleep(100)                                               ; give focus to OK
Send("{Enter}")                                          ; press OK
Sleep(1000)

; ── STEP 4: Build TFT Output ───────────────── 
; Activate the editor window just in case
WinActivate("Nextion Editor")
Sleep(100)

;Click the File menu — get coordinates using Window Spy
Click(23, 46)         ; adjust this to your File menu position
Sleep(200)            ; let the menu open

;Navigate to “TFT Output” using arrow keys
Send("{Down 1}")      ; move to first item — adjust if needed
Sleep(100)
Send("{Enter}")       ; trigger TFT Output
Sleep(1000)

;WinWaitActive("ahk_class #32770")
Send("{Tab 2}")
SendText(outputTftDir1)
Sleep(300)
Send("{Tab}{Tab}{Enter}") ; Go to output and confirm
; ── Save dialog handled, output created ──
Sleep(2000)              ; small buffer while Explorer opens
WinWaitActive("ahk_class CabinetWClass", , 3) ; optional: wait for folder
Send("!{F4}")           ; close the newly opened folder window
Sleep(2000)              ; small buffer while Explorer opens

;TFT2-------------------------------------------------------------------------------TFT2

; ── STEP 2: Delete all images ────────────────
Click(195, 444)        ; Trash icon
Sleep(500)
Send("{Enter}")        ; Confirm delete
Sleep(1000)

; ── STEP 3: Add new pictures ─────────────────
Click(27, 444)         ; Plus icon (Add)
;WinWaitActive("Add Pictures")

WinWaitActive("ahk_class #32770")      ; the file-chooser window

Send("!d")              ; focus the path field
Sleep(100)
SendText(inputImageDir2)
Send("{Enter}")         ; navigate
Sleep(800)                                   ; let the folder refresh

;Shift focus into the file list
Send("{Tab 4}")                             ; 4x Tab to reach the file list
Sleep(100)
Send("^a")                                  ; Select all files
Sleep(100)
Send("{Enter}")                             ; Import into Nextion

WinWaitClose("ahk_class #32770", , 5)
Sleep(3000)
; wait for the success pop-up and dismiss it
;WinWaitActive("Nextion Editor", "Import successful", 5)  ; title + text filter
;Sleep(100)                                               ; give focus to OK
Send("{Enter}")                                          ; press OK
Sleep(1000)

; ── STEP 4: Build TFT Output ───────────────── 
; Activate the editor window just in case
WinActivate("Nextion Editor")
Sleep(100)

;Click the File menu — get coordinates using Window Spy
Click(23, 46)         ; adjust this to your File menu position
Sleep(200)            ; let the menu open

;Navigate to “TFT Output” using arrow keys
Send("{Down 1}")      ; move to first item — adjust if needed
Sleep(100)
Send("{Enter}")       ; trigger TFT Output
Sleep(1000)

;WinWaitActive("ahk_class #32770")
Send("{Tab 2}")
SendText(outputTftDir2)
Sleep(300)
Send("{Tab}{Tab}{Enter}") ; Go to output and confirm
; ── Save dialog handled, output created ──
Sleep(2000)              ; small buffer while Explorer opens
WinWaitActive("ahk_class CabinetWClass", , 3) ; optional: wait for folder
Send("!{F4}")           ; close the newly opened folder window
Sleep(2000)              ; small buffer while Explorer opens

;TFT3---------------------------------------------------------------TFT3; ── STEP 2: Delete all images ────────────────
Click(195, 444)        ; Trash icon
Sleep(500)
Send("{Enter}")        ; Confirm delete
Sleep(1000)

; ── STEP 3: Add new pictures ─────────────────
Click(27, 444)         ; Plus icon (Add)
;WinWaitActive("Add Pictures")

WinWaitActive("ahk_class #32770")      ; the file-chooser window

Send("!d")              ; focus the path field
Sleep(100)
SendText(inputImageDir3)
Send("{Enter}")         ; navigate
Sleep(800)                                   ; let the folder refresh

;Shift focus into the file list
Send("{Tab 4}")                             ; 4x Tab to reach the file list
Sleep(100)
Send("^a")                                  ; Select all files
Sleep(100)
Send("{Enter}")                             ; Import into Nextion

WinWaitClose("ahk_class #32770", , 5)
Sleep(3000)
; wait for the success pop-up and dismiss it
;WinWaitActive("Nextion Editor", "Import successful", 5)  ; title + text filter
;Sleep(100)                                               ; give focus to OK
Send("{Enter}")                                          ; press OK
Sleep(1000)

; ── STEP 4: Build TFT Output ───────────────── 
; Activate the editor window just in case
WinActivate("Nextion Editor")
Sleep(100)

;Click the File menu — get coordinates using Window Spy
Click(23, 46)         ; adjust this to your File menu position
Sleep(200)            ; let the menu open

;Navigate to “TFT Output” using arrow keys
Send("{Down 1}")      ; move to first item — adjust if needed
Sleep(100)
Send("{Enter}")       ; trigger TFT Output
Sleep(1000)

;WinWaitActive("ahk_class #32770")
Send("{Tab 2}")
SendText(outputTftDir3)
Sleep(300)
Send("{Tab}{Tab}{Enter}") ; Go to output and confirm
; ── Save dialog handled, output created ──
Sleep(2000)              ; small buffer while Explorer opens
WinWaitActive("ahk_class CabinetWClass", , 3) ; optional: wait for folder
Send("!{F4}")           ; close the newly opened folder window
Sleep(2000)              ; small buffer while Explorer opens

;TFT4---------------------------------------------------------------TFT4; ── STEP 2: Delete all images ────────────────
Click(195, 444)        ; Trash icon
Sleep(500)
Send("{Enter}")        ; Confirm delete
Sleep(1000)

; ── STEP 3: Add new pictures ─────────────────
Click(27, 444)         ; Plus icon (Add)
;WinWaitActive("Add Pictures")

WinWaitActive("ahk_class #32770")      ; the file-chooser window

Send("!d")              ; focus the path field
Sleep(100)
SendText(inputImageDir4)
Send("{Enter}")         ; navigate
Sleep(800)                                   ; let the folder refresh

;Shift focus into the file list
Send("{Tab 4}")                             ; 4x Tab to reach the file list
Sleep(100)
Send("^a")                                  ; Select all files
Sleep(100)
Send("{Enter}")                             ; Import into Nextion

WinWaitClose("ahk_class #32770", , 5)
Sleep(3000)
; wait for the success pop-up and dismiss it
;WinWaitActive("Nextion Editor", "Import successful", 5)  ; title + text filter
;Sleep(100)                                               ; give focus to OK
Send("{Enter}")                                          ; press OK
Sleep(1000)

; ── STEP 4: Build TFT Output ───────────────── 
; Activate the editor window just in case
WinActivate("Nextion Editor")
Sleep(100)

;Click the File menu — get coordinates using Window Spy
Click(23, 46)         ; adjust this to your File menu position
Sleep(200)            ; let the menu open

;Navigate to “TFT Output” using arrow keys
Send("{Down 1}")      ; move to first item — adjust if needed
Sleep(100)
Send("{Enter}")       ; trigger TFT Output
Sleep(1000)

;WinWaitActive("ahk_class #32770")
Send("{Tab 2}")
SendText(outputTftDir4)
Sleep(300)
Send("{Tab}{Tab}{Enter}") ; Go to output and confirm
; ── Save dialog handled, output created ──
Sleep(2000)              ; small buffer while Explorer opens
WinWaitActive("ahk_class CabinetWClass", , 3) ; optional: wait for folder
Send("!{F4}")           ; close the newly opened folder window
Sleep(2000)              ; small buffer while Explorer opens

ExitApp
