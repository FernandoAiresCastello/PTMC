
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
