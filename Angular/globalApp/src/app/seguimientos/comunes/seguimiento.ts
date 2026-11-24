import { Utilidades } from '../../utilidades/utilidades';

export interface Seguimiento {
  id?: number;
  fecha?: Date;
  supervisor?: string;
  observado?: string;
  observacion?: string;
  calificacion?: string;
  idsupervisor?: number;
  area?: string;
  sector?: string;  // alias para area
  idobservado?: number;
  detalle?: string;
  nombre?: string;
  apellido?: string;
  idsector?: number;
}

export interface EmpleadoParaSeguir {
  id: number;
  nombre: string;
  sector: number;
}

export interface TipoDeObservacion {
  id: number;
  nombre: string;
}


export interface HistorialSeguimiento {
  id?: number;
  fecha?: Date;
  tipoobservacion?: number;
  calificacion?: string;
  idsector?: number;
  sector?: string;
  detalle?: string;
  observacion?: string;
  supervisor?: string;
}

export class UtilesSeguimientos {
  public static calificaciones = [
    {
      id: 0,
      detalle: 'Neutra',
      valor: 'N'
    },
    {
      id: 1,
      detalle: 'Positiva',
      valor: '+'
    },
    {
      id: 2,
      detalle: 'Negativa',
      valor: '-'
    }
  ];

  constructor() {
  }

}

