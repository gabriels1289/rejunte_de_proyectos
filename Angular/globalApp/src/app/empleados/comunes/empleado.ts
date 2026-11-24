export class Empleado {
  id?: number;
  legajo?: number;
  apellido?: string;
  nombre?: string;
  direccion?: string;
  localidad?: string;
  codigopostal?: string;
  telefonofijo?: string;
  telefonomovil?: string;
  mail?: string;
  fechanacimiento?: Date;
  fechaincorporacion?: Date;
  fechacontrato?: Date;
  tipodocumento?: string;
  numerodocumento?: string;
  cuil?: string;
  legajoestudio?: number;
  preocupacional?: string;
  preocupacionaldetalle?: string;
  fechaanalisis?: Date;
  basico?: number;
  sexo?: string;
  fechabaja?: Date;
  activo?: string;
  fulltime?: string;
  foto?: string;
  idart?: number;
  art?: string;
  idobrasocial?: number;
  obrasocial?: string;
  idsector?: number;
  sector?: string;
  idacceso?: number;
  acceso?: string;
  idempresa?: number;
  razonsocial?: string;
  username?: string;
  password?: string;
  token?: string;
}

export interface EmpleadoListado {
  id: number;
  legajo: number;
  nombre: string;
  apellido: string;
  sector: string;
  foto: string;
  activo: string;
}

export interface Sexo {
  valor: string;
  descripcion: string;
}

// TODO: implementar reporte, buscar complemento para crear pdf
