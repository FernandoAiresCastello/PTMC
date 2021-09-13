
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
