import { Component } from '@angular/core';

import { AuthService } from './auth/auth.service';
import { Router } from '@angular/router';
import { Rol } from './_models/roles';
import { User } from './_models/user';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'globalApp';
  currentUser: User;

  constructor(private router: Router, private authService: AuthService) {
    this.authService.currentUser.subscribe(x => this.currentUser = x);
  }

  // TODO: utilizar funciones de obtencion de roles para ocultar elementos de navegacion
  // ejemplo:
  get isAdmin() {
    return this.currentUser && this.currentUser.rol === Rol.Sistemas;
  }

  get tienePermisoMenuEmpleados() {
    // tslint:disable: max-line-length
    if (this.currentUser.idacceso === 1 || this.currentUser.idacceso === 3 || this.currentUser.idacceso === 6 || this.currentUser.idacceso === 8 ||
      this.currentUser.idacceso === 9 || this.currentUser.idacceso === 10 || this.currentUser.idacceso === 13 || this.currentUser.idacceso === 17 ||
      this.currentUser.idacceso === 18 || this.currentUser.idacceso === 21 || this.currentUser.idacceso === 22) {
      return true;
    } else {
      return false;
    }
    // tslint:enable: max-line-length
  }

  get tienePermisoMenuEvaluaciones() {
    // tslint:disable: max-line-length
    if (this.currentUser.idacceso === 1 || this.currentUser.idacceso === 6 || this.currentUser.idacceso === 8 || this.currentUser.idacceso === 9 ||
      this.currentUser.idacceso === 10 || this.currentUser.idacceso === 17) {
      return true;
    } else {
      return false;
    }
    // tslint:enable: max-line-length
  }

  logout() {
    this.authService.logout();
    this.router.navigate(['/login']);
  }

  // TODO: desuscribirme
  // TODO: formatear fechas
}
