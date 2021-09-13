
class GraphicsDriver {
    Canvas = null;
    Overlay = null;
    Tileset = null;
    Palette = null;
    BackColor = 0;
    BorderColor = 7;
    TextColor = 1;
    ScreenWidth = 160;
    ScreenHeight = 144;
    ScreenZoom = 4;
    TilePxCountX = 8;
    TilePxCountY = 8;
    Cols = this.ScreenWidth / this.TilePxCountX;
    Rows = this.ScreenHeight / this.TilePxCountY;
    CanvasWidth = this.ScreenZoom * this.ScreenWidth;
    CanvasHeight = this.ScreenZoom * this.ScreenHeight;
    TilePxCount = this.TilePxCountX * this.TilePxCountY;
    GridTileWidth = this.CanvasWidth / this.Cols;
    GridTileHeight = this.CanvasHeight / this.Rows;
    TilePxWidth = this.GridTileWidth / this.TilePxCountX;
    TilePxHeight = this.GridTileHeight / this.TilePxCountY;

    constructor() {
        this.Init();
    }

    Init() {
        this.Tileset = new Tileset();
        this.Palette = new Palette();

        document.body.style.backgroundColor = this.Palette.Colors[this.BorderColor].RGB;
        document.body.style.color = this.Palette.Colors[this.TextColor].RGB;

        const canvasElement = document.getElementsByTagName('canvas')[0];
        canvasElement.width = this.CanvasWidth;
        canvasElement.height = this.CanvasHeight;
        this.Canvas = canvasElement.getContext('2d');
        this.Canvas.imageSmoothingEnabled = false;

        this.Overlay = document.getElementById('overlay');

        this.ClearBackground();
    }

    ClearBackground() {
        this.Canvas.fillStyle = this.Palette.Colors[this.BackColor].RGB;
        this.Canvas.fillRect(0, 0, this.CanvasWidth, this.CanvasHeight);
    }

    RenderPixel(color, x, y) {
        if (color != null) {
            this.Canvas.strokeStyle = '';
            this.Canvas.fillStyle = this.Palette.Colors[color].RGB;
            this.Canvas.fillRect(x * this.TilePxWidth, y * this.TilePxHeight, this.TilePxWidth, this.TilePxHeight);
        }
    }
    
    RenderPixelBlock(pixelBlock, fgc, bgc, x, y) {
        x *= this.TilePxCountX;
        y *= this.TilePxCountY;
        let px = x;
        let py = y;
        const pixels = pixelBlock.Pixels;
        for (let i = 0; i < this.TilePxCount; i++) {
            this.RenderPixel(((pixels[i] == '1') ? fgc : (pixels[i] == '0') ? bgc : null), px, py);
            if (++px >= x + this.TilePxCountX) {
                px = x;
                py++;
            }
        }
    }

    RenderTile(tileIndexOrChar, fgc, bgc, x, y) {
        if (tileIndexOrChar == null) {
            throw new Error('tileIndexOrChar is null');
        }
        
        let pixelBlock = null;
        if (Number.isInteger(tileIndexOrChar)) {
            pixelBlock = this.Tileset.Tiles[tileIndexOrChar];
        }
        else if (tileIndexOrChar.charCodeAt && tileIndexOrChar.length == 1) {
            pixelBlock = this.Tileset.Tiles[tileIndexOrChar.charCodeAt(0)]
        }
        else {
            throw new Error('tileIndexOrChar must be an integer or a single character string');
        }

        this.RenderPixelBlock(pixelBlock, fgc, bgc, x, y);
    }
}
