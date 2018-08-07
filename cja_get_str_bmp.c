
// This is a function I wrote to allow for keyboard input when in 'bitmap' mode.
// It provides auto-repeat, debounce, arrowing-around within input area,
// asterisks-over-passwords, and can be passed in a ptr to a string to be 
// used as a default value. It also returns special "escape" keys such 
// as FCTN-BACK, DEL, etc to the caller.

char cja_get_str_bmp(int row, int col, char *buf, int maxlen, int passwd_flag) {
    int x = 0;
    int y = 0;
    
    int blink_counter = 0; // For blinking cursor
    int blink_status = 1;
    int repeat_counter = 0;
    
    int original_col = col;
    
    int mark = strlen(buf);
    char last_key;  // The last key pressed 
    
    char mystr[20];  // For displaying ints on screen
    if (passwd_flag) {  // Blank out passwords
        mark = 0;
        buf[0] = '\0';
    }
    if (strlen(buf)) {
        writestring_bmp(row, col, buf);
        
        col += strlen(buf);
    }
    while (1) {
        kscan(0);
        __asm__("limi 2\n");  // Enable interrupts just for a split second, to trap people pressing FCTN-QUIT and allowing the screen to blank after idle.
        __asm__("limi 0\n");  // Disable interrupts
    
        if (++blink_counter > 200) {
            blink_counter = 0;
            if (blink_status)
                blink_status = 0;
            else
                blink_status = 1;
        }
        if (blink_status)
            bm_placePattern(col, row, PAT95);
        else 
            bm_placePattern(col, row, PAT32);
        switch (KSCAN_KEY) {
            case 13:
                bm_placePattern(col, row, PAT32);
                buf[mark] = '\0';
                
                delaytipi(40);  // A little debounce
                return KSCAN_KEY;
                
            case 15:            // FCTN-BACK pressed
                buf[0] = '\0';
                
                for (x = original_col; x <= original_col + maxlen; x++)
                    bm_placePattern(x, row, PAT32);
                
                delaytipi(20);  // A little debounce
                return KSCAN_KEY;
            case 3:            // FCTN-DEL pressed
                return 3;
 
            case 8:             // Left arrow (FCTN-S), Treated as Backspace.
                if (mark > 0) {
                    if (KSCAN_KEY == last_key) {
                        repeat_counter++;
                        if (++repeat_counter <= 620) {
                            continue;    
                        }
                        
                        repeat_counter = 620; // Keep from overflowing
                        delaytipi(120);
                    }
                    bm_placePattern(col--, row, PAT32);
                    mark--;
                    last_key = KSCAN_KEY;
                }
                break;
            case 255: // No key pressed
                repeat_counter = 0;
                last_key = '\0';
                continue;
                
            default:
                if ((KSCAN_KEY >= 48 && KSCAN_KEY <= 57) || (KSCAN_KEY >= 65 && KSCAN_KEY <= 90) || KSCAN_KEY == 46 || KSCAN_KEY == 58 || KSCAN_KEY == 95) { // Key pressed
                
                    if (mark >= maxlen)
                        continue;
                
                    if (KSCAN_KEY == last_key) {
                        repeat_counter++;
                        if (++repeat_counter <= 620) {
                            continue;    
                        }
                        repeat_counter = 620; // Keep from overflowing
                        delaytipi(120);
                    }
                    else {
                        repeat_counter = 0;
                    }                  
                  
                    buf[mark++] = KSCAN_KEY;
                    if (passwd_flag) {
                        bm_placePattern(col++, row, PAT42);  // Also need to check that we're within ASCII range
                    }
                    else if (KSCAN_KEY == 32) {
                        bm_placePattern(col++, row, PAT32);
                    }
                    else if ( (KSCAN_KEY >= 48 && KSCAN_KEY <= 57) || (KSCAN_KEY >= 65 && KSCAN_KEY <= 90) || KSCAN_KEY == 46 || KSCAN_KEY == 58 || KSCAN_KEY == 95) {
                        mystr[0] = KSCAN_KEY;
                        mystr[1] = '\0';
                    
                        writestring_bmp(row, col++, mystr);
                    }                        
                    last_key = KSCAN_KEY;
                }
                
                break;
                    
        }  
    }
}
