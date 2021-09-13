
class Color {
    RGB = null;
    
    constructor(rgb) {
        this.RGB = '#' + rgb.toString(16).padStart(6, '0');
    }
}
