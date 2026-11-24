import { Utilidades } from '../../utilidades/utilidades';

export interface Modelo {
  id?: number;
  idcompetencia?: number;
  competencia?: string;
  idcomportamiento?: number;
  comportamiento?: string;
  promedio?: number;
}

export interface Comportamiento {
  id: number;
  comportamiento: string;
}

export interface Competencia {
  id: number;
  competencia: string;
}

export interface EvaluacionesListado {
  id?: number;
  titulo?: string;
  sector?: string;
  fecha?: string;
  activa?: string;
  idsector?: number;
  modelo?: number;
  orden?: number;
  creadapor?: number;
}

export class UtilesEvaluacion {
  constructor() {
  }


  /* public static decodificarString(str: string) {
    return $('<textarea/>').html(str).text();
  } */

  public static cambiarfondo(valor, formatoViejo): string {
    if (formatoViejo) {
      // formato viejo
      if (valor <= 30) {
        return '#FF0000'; // inferior a lo esperado
      } else if ( valor <= 40 && valor > 30) {
        return '#FF5100'; // bajo
      } else if (valor <= 50 && valor > 40) {
        return '#FF7C00'; // necesita mejorar
      } else if (valor <= 60 && valor > 50) {
        return '#FFCD00'; // medio
      } else if (valor <= 70 && valor > 60) {
        return '#FFE600'; // bueno
      } else if (valor <= 80 && valor > 70) {
        return '#C9FF00'; // superior
      } else if (valor <= 90 && valor > 80) {
        return '#49FF00'; // destacado
      } else if (valor <= 100 && valor > 90) {
        return '#34C215'; // excepcional
      }
    } else {
      // formato nuevo
      if (valor <= 29) {
        return '#FF0000'; // bajo
      } else if ( valor <= 59 && valor > 29) {
        return '#FF7C00'; // necesita mejorar
      } else if (valor <= 69 && valor > 59) {
        return '#FFE600'; // bueno
      } else if (valor <= 90 && valor > 69) {
        return '#A2FF00'; // muy bueno
      } else if (valor <= 100 && valor > 90) {
        return '#34C215'; // excelente
      }
    }
  }

  public static calcularPorcentajesVisual(competencias, modelos) {
    for (const competencia of competencias) {
      competencia.porcentaje = 0;
    }

    for (const competencia of competencias) {
      let cantidad = 0;
      for (const modelo of modelos) {
        if (competencia.competencia === modelo.competencia && (modelo.promedio !== -1)) {
          cantidad++;
          competencia.porcentaje += modelo.promedio;
        }
      }
      competencia.porcentaje /= cantidad;
    }
    return competencias;
  }

  public static calcularPromedioVisual(modelos): number {
    let total = 0;
    let cantidad = 0;
    for (const modelo of modelos) {
      if (modelo.promedio !== -1) {
        cantidad++;
        total += modelo.promedio;
      }
    }
    total /= cantidad;
    return total;
  }


}
