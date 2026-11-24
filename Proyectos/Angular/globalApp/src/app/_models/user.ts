import { Rol } from './roles';

export class User {
  id: number;
  nombre: string;
  apellido: string;
  area: string;
  idarea: number;
  correorestringido?: number;
  foto?: string;
  idacceso: number;
  rol: Rol;
  idaccesopadre: number;
  listaIdHijos?: string;
  token?: string;
  username: string;
  password?: string;
}
