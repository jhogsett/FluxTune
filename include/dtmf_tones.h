#ifndef __DTMF_TONES_H__
#define __DTMF_TONES_H__

// DTMF tone frequencies (Hz)
// DTMF uses a 4x4 matrix of row and column tones

// Row tones (low frequencies)
const float DTMF_ROW_TONES[4] = {
    697.0,   // Row 0
    770.0,   // Row 1
    852.0,   // Row 2
    941.0    // Row 3
};

// Column tones (high frequencies)  
const float DTMF_COL_TONES[4] = {
    1209.0,  // Col 0
    1336.0,  // Col 1
    1477.0,  // Col 2
    1633.0   // Col 3
};

// Get DTMF tones by row and column indices (0-3 each)
// This is the core function - no keypad mapping needed
inline void get_dtmf_tones_by_indices(unsigned char row, unsigned char col, float &row_freq, float &col_freq) {
    row_freq = DTMF_ROW_TONES[row & 0x03];  // Mask to ensure 0-3 range
    col_freq = DTMF_COL_TONES[col & 0x03];  // Mask to ensure 0-3 range
}

// Optional: Generate random DTMF tone pair for pager simulation
inline void get_random_dtmf_tones(float &row_freq, float &col_freq) {
    unsigned char row = rand() & 0x03;  // Random 0-3
    unsigned char col = rand() & 0x03;  // Random 0-3
    get_dtmf_tones_by_indices(row, col, row_freq, col_freq);
}

// Optional: Keypad character mapping (only if needed for user input)
// Returns true if valid, false if invalid character
inline bool get_dtmf_tones_from_char(char c, float &row_freq, float &col_freq) {
    // Convert to uppercase
    if (c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    }
    
    // Map keypad characters to row/col (if really needed)
    unsigned char row, col;
    switch (c) {
        case '1': row = 0; col = 0; break;
        case '2': row = 0; col = 1; break;
        case '3': row = 0; col = 2; break;
        case 'A': row = 0; col = 3; break;
        case '4': row = 1; col = 0; break;
        case '5': row = 1; col = 1; break;
        case '6': row = 1; col = 2; break;
        case 'B': row = 1; col = 3; break;
        case '7': row = 2; col = 0; break;
        case '8': row = 2; col = 1; break;
        case '9': row = 2; col = 2; break;
        case 'C': row = 2; col = 3; break;
        case '*': row = 3; col = 0; break;
        case '0': row = 3; col = 1; break;
        case '#': row = 3; col = 2; break;
        case 'D': row = 3; col = 3; break;
        default: return false;  // Invalid character
    }
    
    get_dtmf_tones_by_indices(row, col, row_freq, col_freq);
    return true;
}

#endif
