
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
