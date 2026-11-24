export class Utilidades {
  constructor() {
  }

  /**
   * Esta función filtra los duplicados de un arreglo de objetos por id
   * Devuelve un arreglo de objetos sin elementos duplicados
   * @param arreglo arreglo de objetos a trabajar
   */
  public static eliminarDuplicados(arreglo: any[]): any[] {
    const arr = {};

    for (let i = 0, len = arreglo.length; i < len; i++) {
      // tslint:disable-next-line: no-string-literal
      arr[arreglo[i]['id']] = arreglo[i];
    }

    arreglo = new Array();
    for (const key in arr) {
      if (arr.hasOwnProperty(key)) {
        arreglo.push(arr[key]);
      }
    }
    return arreglo;
  }

}
