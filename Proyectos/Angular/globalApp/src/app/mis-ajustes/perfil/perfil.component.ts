import { Component, OnInit } from '@angular/core';
import { PopupInformativoComponent, PopupInformativoModel } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { DialogoConfirmacionComponent, ConfirmDialogModel } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { AuthService } from 'src/app/auth/auth.service';
import { MatDialog, MatDialogConfig } from '@angular/material';
import { EmpleadosService } from 'src/app/empleados/servicios/empleados.service';
import { ActivatedRoute, Router } from '@angular/router';
import { Empleado } from 'src/app/empleados/comunes/empleado';
import { CambiarPasswordComponent } from '../cambiar-password/cambiar-password.component';

@Component({
  selector: 'app-perfil',
  templateUrl: './perfil.component.html',
  styleUrls: ['./perfil.component.css']
})
export class PerfilComponent implements OnInit {

  public spinner = true;
  private idempleado;
  public empleado$: Empleado;
  public idacceso;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private empleadosService: EmpleadosService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    this.idempleado = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
  }

  ngOnInit() {
    this.obtenerDetalles();
  }

  obtenerDetalles() {
    this.empleadosService.getDetalleEmpleado(this.idempleado).subscribe(
      (res) => {
        // console.log('Res: %o', res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.empleado$ = res['body'];
          this.spinner = false;
        } else {
          this.spinner = false;
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  cambiarPassword() {
    const dialogConfig = new MatDialogConfig();

    dialogConfig.disableClose = true;
    dialogConfig.autoFocus = true;

    // dialogConfig.data = aux;

    const dialogRef = this.dialog.open(CambiarPasswordComponent, dialogConfig);

    dialogRef.afterClosed().subscribe(
      (res) => {
        if (res === 'ok') {
          // cambio efetuado con éxito
        }
      }
    );
  }

}
