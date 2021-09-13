
class Util {

    static ByteToBinaryString(value) {
        return value.toString(2).padStart(8, '0');
    }
}
