import { Utilidades } from '../../utilidades/utilidades';
import * as $ from 'jquery';

export interface Correo {
  id?: number;
  remitente?: number;
  asunto?: string;
  cuerpo?: string;
  cuerpo2?: string; // string sin los tags html
  fecha?: Date;
  lecturas?: number;
  respuestas?: number;
  estado?: string;
  leido?: string;
  destacado?: string;
  prioridad?: number;
  area?: string;
  nombreapellido?: string;
  foto?: string;
  fechamsj?: Date;
  esRespuesta?: string;
}

export interface Usuario {
  id: number;
  usuario: string;
  area: string;
}

export interface Area {
  id: number;
  area: string;
}

export class UtilesMensajeria {
  constructor() {
  }

  private static decode(str: string) {
    return str.replace(/&#(\d+);/g, (match, dec) => {
    return String.fromCharCode(dec);
    });
  }

  // funciones auxiliares para quitar los tags html del cuerpo
  public static acomodarString(str: string) {
    str = str.replace(/<[^>]*>?/g, ' ');	// quita los tags html
    str = str.replace(/^\s+|\s+$|\s+(?=\s)/g, ''); // elimina todos los espacios en blanco multiples y los del principio y final
    str = this.decode(str);
    return str;
  }

  public static concatenarDestinatarios(destinatarios: any): string {
    let textoDestinarios = '';
    if (destinatarios !== 'consulta vacia') {
      for (const dest of destinatarios) {
        textoDestinarios += (dest.area + ', ');
      }
    }
    return textoDestinarios;
  }

  public static htmlDecode(str: string) {
    const doc = new DOMParser().parseFromString(str, 'text/html');
    return doc.documentElement.textContent;
  }

  public static decodificarString(str: string) {
    return $('<textarea/>').html(str).text();
  }

  public static generarDestinatarios(arregloDeUsuarios: Usuario[], arregloDeAreas: Area[], todosLosUsuarios: Usuario[]): any[] {
    let destinos = [];
    const temp = [];
    // obtengo los usuarios pertencientes a las areas seleccionadas
    for (const itemArea of arregloDeAreas) {
      for (const itemUsuario of todosLosUsuarios) {
        if (itemUsuario.area === itemArea.area) {
          temp.push({ id: itemUsuario.id, usuario: itemUsuario.usuario });
        }
      }
    }

    // agrego en la variable destinos los usuarios pertenecientes a las areas
    for (const item of temp) {
      destinos.push({ id: item.id, usuario: item.usuario });
    }
    // agrego en la variable destinos los usuarios seleccionados
    for (const item of arregloDeUsuarios) {
      destinos.push({ id: item.id, usuario: item.usuario });
    }

    // filtro los usuarios duplicados
    destinos = Utilidades.eliminarDuplicados(destinos);

    return destinos;
  }

}
