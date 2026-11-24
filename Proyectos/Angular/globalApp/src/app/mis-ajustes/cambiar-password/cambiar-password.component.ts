import { Component, OnInit, Inject } from '@angular/core';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { MatDialog, MatDialogRef, MAT_DIALOG_DATA } from '@angular/material';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { AuthService } from 'src/app/auth/auth.service';
import { MisAjustesService } from '../servicios/mis-ajustes.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { Router } from '@angular/router';

@Component({
  selector: 'app-cambiar-password',
  templateUrl: './cambiar-password.component.html',
  styleUrls: ['./cambiar-password.component.css']
})
export class CambiarPasswordComponent implements OnInit {

  spinner = false;
  form: FormGroup;
  idusuario: number;

  constructor(
    private dialog: MatDialog,
    private fb: FormBuilder,
    private authService: AuthService,
    private ajustesService: MisAjustesService,
    private router: Router,
    public dialogRef: MatDialogRef<CambiarPasswordComponent>,
    @Inject(MAT_DIALOG_DATA) data
  ) {
    this.idusuario = this.authService.currentUserValue.id;
    this.form = fb.group({
      oldPassCtrl: ['', [Validators.required, Validators.minLength(6), Validators.maxLength(40)]],
      newPassCtrl: ['', [Validators.required, Validators.minLength(6), Validators.maxLength(40)]],
      confirmPassCtrl: ['', [Validators.required, Validators.minLength(6), Validators.maxLength(40)]],
    });
  }

  ngOnInit() { }

  close() {
    this.dialogRef.close();
  }

  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessageMinMaxRequired(control: string) {
    return this.form.get(control).hasError('required') ? 'Este campo es obligatorio' :
        this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }

  get deshabilitado() {
    if (this.form.invalid || (this.f.newPassCtrl.value !== this.f.confirmPassCtrl.value)) {
      return true;
    } else {
      return false;
    }
  }

  logout() {
    this.authService.logout();
    this.router.navigate(['/login']);
  }

  save() {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef3 = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef3.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;

        const data = {
          idusuario: this.idusuario,
          newpassword: this.f.newPassCtrl.value,
          oldpassword: this.f.oldPassCtrl.value
        };

        this.ajustesService.putPassword(JSON.stringify(data)).subscribe(
          (res) => {
            this.spinner = false;
            // tslint:disable: no-string-literal
            if (res['status'] === 200) {
              const message2 = `Cambio efectuado con éxito`;
              const dialogData2 = new PopupInformativoModel('Éxito', message2, 'success');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});
              dialogRef2.afterClosed().subscribe(
                resultado => {
                  this.close();
                  this.logout();
                }
              );
            } else if (res['status'] === 204) {
              const message2 = 'La contraseña actual ingresada no es correcta';
              const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});
            }
            // tslint:enable: no-string-literal
          }
        );
      }
    });
  }

}
