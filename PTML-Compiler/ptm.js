/*=============================================================================

        PTM - Programmable Tile Machine
        2021 developed by Fernando Aires Castello
        https://github.com/FernandoAiresCastello
    
=============================================================================*/

String.prototype.replaceAt = function (index, replacement) {
    return this.substr(0, index) + replacement + this.substr(index + replacement.length);
}

/*===========================================================================*/

class Util {

    static ByteToBinaryString(value) {
        return value.toString(2).padStart(8, '0');
    }
}

/*===========================================================================*/

class Random {

    static Number(max) {
        return Math.floor(Math.random() * max);
    }

    static Id() {
        const alphabet = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
        const length = 8;
        var id = '';
        for (var i = 0; i < length; i++) {
            id += alphabet.charAt(Random.Number(alphabet.length));
        }
        return id;
    }
}

/*===========================================================================*/

class PixelBlock {
    Width = 8;
    Height = 8;
    Length = this.Width * this.Height;
    Pixels = null;

    constructor(row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Set(row0, row1, row2, row3, row4, row5, row6, row7);
    }

    Set(row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Pixels = '';
        this.Pixels += Util.ByteToBinaryString(row0);
        this.Pixels += Util.ByteToBinaryString(row1);
        this.Pixels += Util.ByteToBinaryString(row2);
        this.Pixels += Util.ByteToBinaryString(row3);
        this.Pixels += Util.ByteToBinaryString(row4);
        this.Pixels += Util.ByteToBinaryString(row5);
        this.Pixels += Util.ByteToBinaryString(row6);
        this.Pixels += Util.ByteToBinaryString(row7);
    }

    SetRow(row, value) {
        const pos = row * this.Width;
        let pixels = Util.ByteToBinaryString(value);
        for (let i = 0; i < this.Width; i++)
            this.Pixels = this.Pixels.replaceAt(pos + i, pixels[i]);
    }

    SetBlank() {
        this.Set(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

/*===========================================================================*/

class Tileset {
    Tiles = [];

    constructor() {
        this.InitDefault();
    }

    Add(row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Tiles.push(new PixelBlock(row0, row1, row2, row3, row4, row5, row6, row7));
    }

    AddBlank() {
        this.Tiles.push(new PixelBlock(0, 0, 0, 0, 0, 0, 0, 0));
    }

    Set(index, row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Tiles[index].Set(row0, row1, row2, row3, row4, row5, row6, row7);
    }

    SetRow(index, row, value) {
        this.Tiles[index].SetRow(row, value);
    }

    SetBlank(index) {
        this.Tiles[index].SetBlank();
    }

    Get(index) {
        return this.Tiles[index];
    }

    InitDefault() {
        const size = 256;
        for (let i = 0; i < size; i++) {
            this.AddBlank();
        }

        let i = 32;
        this.Set(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        this.Set(i++, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x30, 0x00);
        this.Set(i++, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        this.Set(i++, 0x00, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x00, 0x00);
        this.Set(i++, 0x10, 0xfe, 0xd0, 0xfe, 0x16, 0xd6, 0xfe, 0x10);
        this.Set(i++, 0x00, 0x62, 0x64, 0x08, 0x10, 0x26, 0x46, 0x00);
        this.Set(i++, 0x10, 0x7c, 0x60, 0x38, 0x60, 0x7c, 0x10, 0x00);
        this.Set(i++, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00);
        this.Set(i++, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e);
        this.Set(i++, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70);
        this.Set(i++, 0x10, 0x38, 0x6c, 0x38, 0x10, 0x00, 0x00, 0x00);
        this.Set(i++, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00);
        this.Set(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08);
        this.Set(i++, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00);
        this.Set(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00);
        this.Set(i++, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80);
        this.Set(i++, 0x00, 0xfe, 0xc6, 0xd6, 0xd6, 0xc6, 0xfe, 0x00);
        this.Set(i++, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x66, 0x06, 0x7e, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x06, 0x3c, 0x06, 0x06, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x66, 0x66, 0x66, 0x7e, 0x06, 0x06, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x60, 0x7e, 0x06, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x60, 0x7e, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x66, 0x3c, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x7e, 0x66, 0x66, 0x7e, 0x06, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00);
        this.Set(i++, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x08);
        this.Set(i++, 0x00, 0x06, 0x18, 0x60, 0x18, 0x06, 0x00, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00);
        this.Set(i++, 0x00, 0x60, 0x18, 0x06, 0x18, 0x60, 0x00, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x06, 0x1e, 0x18, 0x00, 0x18, 0x00);
        this.Set(i++, 0xfe, 0x82, 0xba, 0xaa, 0xbe, 0x80, 0xfe, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x60, 0x60, 0x60, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00);
        this.Set(i++, 0x7e, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x7e, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x60, 0x6e, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00);
        this.Set(i++, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00);
        this.Set(i++, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x66, 0x66, 0x6c, 0x78, 0x66, 0x66, 0x66, 0x00);
        this.Set(i++, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x82, 0xc6, 0xee, 0xfe, 0xd6, 0xd6, 0xc6, 0x00);
        this.Set(i++, 0x46, 0x66, 0x76, 0x7e, 0x6e, 0x66, 0x62, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x66, 0x7e, 0x60, 0x60, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x6e, 0x7e, 0x03);
        this.Set(i++, 0x7e, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x00);
        this.Set(i++, 0x7e, 0x66, 0x60, 0x7e, 0x06, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00);
        this.Set(i++, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x66, 0x66, 0x66, 0x24, 0x3c, 0x18, 0x18, 0x00);
        this.Set(i++, 0xc6, 0xc6, 0xd6, 0xd6, 0xfe, 0x6c, 0x6c, 0x00);
        this.Set(i++, 0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66, 0x00);
        this.Set(i++, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x18, 0x18, 0x00);
        this.Set(i++, 0x7e, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x1e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1e);
        this.Set(i++, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01);
        this.Set(i++, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78);
        this.Set(i++, 0x10, 0x38, 0x54, 0x92, 0x10, 0x10, 0x10, 0x00);
        this.Set(i++, 0x08, 0x10, 0x20, 0x7e, 0x20, 0x10, 0x08, 0x00);
        this.Set(i++, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7c, 0x0c, 0x7c, 0x6c, 0x7e, 0x00);
        this.Set(i++, 0x70, 0x30, 0x3e, 0x36, 0x36, 0x36, 0x3e, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x66, 0x60, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x0e, 0x0c, 0x7c, 0x6c, 0x6c, 0x6c, 0x7c, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x66, 0x7e, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x3e, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x6c, 0x6c, 0x7c, 0x0c, 0x7c);
        this.Set(i++, 0x60, 0x60, 0x7c, 0x6c, 0x6c, 0x6c, 0x6e, 0x00);
        this.Set(i++, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x7e, 0x00);
        this.Set(i++, 0x06, 0x00, 0x06, 0x06, 0x06, 0x36, 0x36, 0x3e);
        this.Set(i++, 0x60, 0x60, 0x66, 0x6c, 0x78, 0x66, 0x66, 0x00);
        this.Set(i++, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x00, 0xfe, 0xd6, 0xd6, 0xd6, 0xd6, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x36, 0x36, 0x36, 0x36, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x66, 0x66, 0x66, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x36, 0x36, 0x3e, 0x30, 0x30);
        this.Set(i++, 0x00, 0x00, 0x7c, 0x6c, 0x6c, 0x7c, 0x0c, 0x0e);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x36, 0x30, 0x30, 0x30, 0x00);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x60, 0x7e, 0x06, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x30, 0x7e, 0x30, 0x30, 0x30, 0x3e, 0x00);
        this.Set(i++, 0x00, 0x00, 0x6c, 0x6c, 0x6c, 0x6c, 0x7e, 0x00);
        this.Set(i++, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00);
        this.Set(i++, 0x00, 0x00, 0xd6, 0xd6, 0xd6, 0xfe, 0x6c, 0x00);
        this.Set(i++, 0x00, 0x00, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x00);
        this.Set(i++, 0x00, 0x00, 0x76, 0x36, 0x36, 0x3e, 0x06, 0x3e);
        this.Set(i++, 0x00, 0x00, 0x7e, 0x06, 0x18, 0x60, 0x7e, 0x00);
        this.Set(i++, 0x0e, 0x08, 0x08, 0x30, 0x08, 0x08, 0x0e, 0x00);
        this.Set(i++, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18);
        this.Set(i++, 0x70, 0x10, 0x10, 0x0c, 0x10, 0x10, 0x70, 0x00);
    }
}

/*===========================================================================*/

class Color {
    RGB = null;

    constructor(rgb) {
        this.RGB = '#' + rgb.toString(16).padStart(6, '0');
    }
}

/*===========================================================================*/

class Palette {
    Size = 256;
    Colors = [];

    constructor() {
        this.InitDefault();
    }

    Add(rgb) {
        this.Colors.push(new Color(rgb));
    }

    Set(index, rgb) {
        this.Colors[index] = new Color(rgb);
    }

    Get(index) {
        this.Colors[index].RGB;
    }

    Clear() {
        this.Colors = [];
        for (let i = 0; i < this.Size; i++) {
            this.Add(0x000000);
        }
    }

    InitDefault() {
        this.Clear();
        this.Set(0, 0x000000);
        this.Set(1, 0xffffff);
    }
}

/*===========================================================================*/

class Tile {
    Index = 0;
    ForeColor = 0;
    BackColor = 0;

    constructor(ix, fgc, bgc) {
        if (ix == null || fgc == null || bgc == null)
            throw new Error('Tile constructor requires 3 parameters');

        this.Index = ix;
        this.ForeColor = fgc;
        this.BackColor = bgc;
    }

    Copy() {
        return new Tile(this.Index, this.ForeColor, this.BackColor);
    }
}

/*===========================================================================*/

class GraphicsDriver {
    Canvas = null;
    Overlay = null;
    Tileset = null;
    Palette = null;

    ScreenWidth = 160;
    ScreenHeight = 144;
    ScreenZoom = 4;
    BackColor = 0;

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

        this.Overlay = document.getElementById('overlay');
        const canvasElement = document.getElementsByTagName('canvas')[0];
        canvasElement.width = this.CanvasWidth;
        canvasElement.height = this.CanvasHeight;
        this.Canvas = canvasElement.getContext('2d');
        this.Canvas.imageSmoothingEnabled = false;
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

/*===========================================================================*/

class Display {
    Machine = null;
    Gfx = null;
    FrameCounter = 0;
    FrameRendererHandle = null;
    MapViews = [];

    constructor(machine) {
        if (machine == null)
            throw new Error('Display must have a reference to the machine');

        this.Machine = machine;
        this.Gfx = new GraphicsDriver();
    }

    SetBackgroundColor(backColor) {
        if (backColor != null) {
            this.Gfx.BackColor = backColor;
        }
    }

    ClearBackground() {
        this.Gfx.ClearBackground();
    }

    StartRendering() {
        this.FrameCounter = 0;
        this.RenderFrame();
    }

    StopRendering() {
        window.cancelAnimationFrame(this.FrameRendererHandle);
    }

    RenderFrame() {
        try {
            for (let i = 0; i < this.MapViews.length; i++) {
                this.MapViews[i].Render();
            }
            this.FrameCounter++;
            this.FrameRendererHandle = window.requestAnimationFrame(() => this.RenderFrame());
        }
        catch (error) {
            this.Machine.Stop(error);
        }
    }

    AddMapView(view, grid) {
        view.Display = this;
        view.ObjectMap = grid;
        this.MapViews.push(view);
    }
}

/*===========================================================================*/

class MapView {
    Display = null;
    ObjectMap = null;
    X = null;
    Y = null;
    Width = null;
    Height = null;
    ScrollX = 0;
    ScrollY = 0;
    TileAnimationEnabled = true;
    TileAnimationIndex = 0;
    TileAnimationHandle = null;

    constructor(x, y, w, h) {
        if (x == null || y == null || w == null || h == null)
            throw new Error('MapView constructor requires 4 arguments');

        this.X = x;
        this.Y = y;
        this.Width = w;
        this.Height = h;

        this.SetTileAnimationInterval(300);
    }

    Render() {
        if (this.Display == null)
            throw new Error('MapView has no Display');
        if (this.ObjectMap == null)
            throw new Error('MapView has no ObjectMap');

        this.RenderObjectMap();
    }

    RenderObjectMap() {
        for (let i = 0; i < this.ObjectMap.Layers.length; i++) {
            this.RenderObjectLayer(i);
        }
    }

    RenderObjectLayer(layerIndex) {
        const layer = this.ObjectMap.Layers[layerIndex];
        let mapX = this.ScrollX;
        let mapY = this.ScrollY;
        let maxViewX = this.X + this.Width;
        let maxViewY = this.Y + this.Height;

        for (let viewY = this.Y; viewY < maxViewY; viewY++) {
            for (let viewX = this.X; viewX < maxViewX; viewX++) {

                let gameObject = null;
                if (layer.Visible == true && layer.IsWithinBounds(mapX, mapY)) {
                    gameObject = layer.GetObject(mapX, mapY);
                }

                this.RenderGameObject(layerIndex, viewX, viewY, gameObject);
                mapX++;
            }
            mapX = this.ScrollX;
            mapY++;
        }
    }

    RenderGameObject(layerIndex, dispX, dispY, o) {
        const gfx = this.Display.Gfx;
        let tile;

        if (o != null && o.Visible) {
            tile = o.Tiles[this.TileAnimationIndex % o.Tiles.length];
        }
        else if (layerIndex == 0) {
            tile = new Tile(0, gfx.BackColor, gfx.BackColor);
        }

        if (tile) {
            gfx.RenderTile(tile.Index, tile.ForeColor, tile.BackColor, dispX, dispY);
        }
    }

    AdvanceTileAnimation() {
        if (this.TileAnimationEnabled) {
            this.TileAnimationIndex++;
        }
    }

    SetTileAnimationInterval(milliseconds) {
        if (this.TileAnimationHandle != null) {
            window.clearInterval(this.TileAnimationHandle);
        }
        this.TileAnimationHandle = window.setInterval(() =>
            this.AdvanceTileAnimation(), milliseconds);
    }

    ScrollBy(distX, distY) {
        this.ScrollX += distX;
        this.ScrollY += distY;
    }

    ScrollTo(x, y) {
        this.ScrollX = x;
        this.ScrollY = y;
    }

    MoveTo(x, y) {
        this.X = x;
        this.Y = y;
    }

    Resize(width, height) {
        this.Width = width;
        this.Height = height;
    }
}

/*===========================================================================*/

class ObjectMap {
    Layers = [];
    Cols = null;
    Rows = null;

    constructor(layers, cols, rows) {
        if (layers == null || cols == null || rows == null)
            throw new Error('ObjectMap constructor requires 3 arguments');

        this.InitLayers(layers, cols, rows);
    }

    InitLayers(layers, cols, rows) {
        this.Cols = cols;
        this.Rows = rows;

        for (let i = 0; i < layers; i++) {
            this.Layers.push(new ObjectLayer(cols, rows));
        }
    }

    RemoveAllObjects() {
        for (let z = 0; z < this.Layers.length; z++) {
            for (let y = 0; y < this.Rows; y++) {
                for (let x = 0; x < this.Cols; x++) {
                    this.Layers[z].RemoveObject(x, y);
                }
            }
        }
    }
}

/*===========================================================================*/

class ObjectLayer {
    Cells = [];
    Cols = null;
    Rows = null;
    Size = null;
    Visible = true;

    constructor(cols, rows) {
        if (cols == null || rows == null)
            throw new Error('ObjectLayer constructor requires 2 arguments');

        this.InitCells(cols, rows);
    }

    InitCells(cols, rows) {
        this.Cols = cols;
        this.Rows = rows;
        this.Size = cols * rows;

        for (let i = 0; i < this.Size; i++) {
            this.Cells.push(new ObjectCell());
        }
    }

    Fill(o) {
        for (let y = 0; y < this.Rows; y++) {
            for (let x = 0; x < this.Cols; x++) {
                this.SetObject(o, x, y);
            }
        }
    }

    RemoveAllObjects() {
        for (let y = 0; y < this.Rows; y++) {
            for (let x = 0; x < this.Cols; x++) {
                this.RemoveObject(x, y);
            }
        }
    }

    IsWithinBounds(x, y) {
        return x >= 0 && y >= 0 && x < this.Cols && y < this.Rows;
    }

    IsOutOfBounds(x, y) {
        return !this.IsWithinBounds(x, y);
    }

    GetCell(x, y) {
        if (x == null || y == null)
            throw new Error('GetCell requires 2 arguments');
        if (this.IsOutOfBounds(x, y))
            throw new Error(`Object cell X${x} Y${y} is out of bounds`);

        return this.Cells[y * this.Cols + x];
    }

    GetObject(x, y) {
        return this.GetCell(x, y).Object;
    }

    SetObject(o, x, y) {
        const cell = this.GetCell(x, y);
        const replacementObject = o ? o.Copy() : null;
        cell.Object = replacementObject;
    }

    RemoveObject(x, y) {
        this.SetObject(null, x, y);
    }
}

/*===========================================================================*/

class ObjectCell {
    Object = null;

    constructor() {
    }
}

/*===========================================================================*/

class GameObject {
    Id = null;
    Tiles = [];
    Visible = true;
    Properties = new Map();

    constructor(tile) {
        if (tile == null)
            throw new Error('GameObject must contain at least one tile');

        this.Id = Random.Id();
        this.SetTile(tile);
    }

    Copy() {
        const copiedTiles = [];
        for (let i = 0; i < this.Tiles.length; i++) {
            copiedTiles.push(this.Tiles[i].Copy());
        }

        const copiedProperties = new Map();
        this.Properties.forEach((value, key) => {
            copiedProperties.set(key, value);
        });

        const copiedObject = new GameObject(copiedTiles[0]);
        copiedObject.Tiles = copiedTiles;
        copiedObject.Properties = copiedProperties;

        return copiedObject;
    }

    SetTile(tile) {
        this.Tiles = [];
        this.AddTile(tile);
    }

    AddTile(tile) {
        this.Tiles.push(tile);
    }

    SetProperty(prop, value) {
        this.Properties.set(prop, value);
    }

    GetProperty(prop) {
        return this.Properties.get(prop);
    }
}

/*===========================================================================*/

class Machine {
    Running = false;
    Display = null;

    constructor() {
        Machine.Info('Machine instance created');
        this.Display = new Display(this);
    }

    Reset() {
        location.reload();
    }

    static Info(obj) {
        let output = '';
        if (typeof obj === 'object') {
            output = JSON.stringify(obj);
        }
        else {
            output = obj.toString();
        }
        console.log('PTM >> ' + output);
    }

    static Error(error) {
        console.error('PTM >> ' + error);
    }

    static Warning(msg) {
        console.warn('PTM >> ' + msg);
    }

    Run() {
        if (this.Running) {
            Machine.Warning('Main loop is already running');
        }
        else {
            this.Running = true;
            Machine.Info('Main loop started');

            try {
                this.Display.StartRendering();
            }
            catch (error) {
                this.Stop(error);
            }
        }
    }

    Stop(error) {
        this.Running = false;
        if (error) {
            Machine.Error('Main loop stopped abnormally due to:\n' + error.message + '\n' + error.stack);
        }
        else {
            Machine.Info('Main loop stopped normally');
        }
        this.Display.StopRendering();
    }
}

/*=======================[ INTERNAL SYSTEM FUNCTIONS ]=======================*/

function Sys_Init() {
    document.body.style.backgroundColor = '#000000';
    window.ptm = new Machine();
    window.display = ptm.Display;
    window.gfx = display.Gfx;
    window.tileset = gfx.Tileset;
    window.palette = gfx.Palette;
}

function Sys_Error(text) {
    const msg = `ERROR: ${text}`;
    console.error(msg);
    gfx.Overlay.innerHTML = `<span style="color:#f00">${msg}</span>`;
}

function Sys_AssertPaletteIndex(ixPalette) {
    if (ixPalette < 0 || ixPalette >= palette.Colors.length) {
        Sys_Error('Palette index out of range');
        return false;
    }
    return true;
}

function Sys_AssertCharsetIndex(ixCharset) {
    if (ixCharset < 0 || ixCharset >= tileset.Tiles.length) {
        Sys_Error('Charset index out of range');
        return false;
    }
    return true;
}

/*==========================[ PUBLIC API ]===================================*/

function Api_Log(text) {
    console.log(text);
}

function Api_Palette_Set(ixPalette, rgb) {
    if (Sys_AssertPaletteIndex(ixPalette)) {
        palette.Colors[ixPalette] = new Color(rgb);
    }
}

function Api_Display_ClearBackground() {
    display.ClearBackground();
}

function Api_Display_SetBackColor(ixPalette) {
    if (Sys_AssertPaletteIndex(ixPalette)) {
        display.SetBackgroundColor(ixPalette);
    }
}

function Api_Charset_Set(ixCharset, pixelRow, byte) {
    if (Sys_AssertCharsetIndex(ixCharset)) {
        tileset.SetRow(ixCharset, pixelRow, byte);
    }
}

function Api_PutChar(col, row, ixCh, ixPalFg, ixPalBg) {
    if (Sys_AssertCharsetIndex(ixCh) && Sys_AssertPaletteIndex(ixPalFg) && Sys_AssertPaletteIndex(ixPalBg)) {
        gfx.RenderPixelBlock(tileset.Get(ixCh), ixPalFg, ixPalBg, col, row);
    }
}

/*==========================[ USER CODE ]====================================*/

[[[COMPILED_JS]]]