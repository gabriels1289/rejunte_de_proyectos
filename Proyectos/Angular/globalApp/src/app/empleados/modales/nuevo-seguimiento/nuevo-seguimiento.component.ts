import { Component, OnInit, Inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialog } from '@angular/material';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { MensajeriaService } from 'src/app/mensajeria/servicios/mensajeria.service';
import { UtilesSeguimientos, TipoDeObservacion, EmpleadoParaSeguir } from 'src/app/seguimientos/comunes/seguimiento';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { EmpleadosService } from '../../servicios/empleados.service';

@Component({
  selector: 'app-nuevo-seguimiento',
  templateUrl: './nuevo-seguimiento.component.html',
  styleUrls: ['./nuevo-seguimiento.component.css']
})
export class NuevoSeguimientoComponent implements OnInit {

  form: FormGroup;
  seguimientoDeSala: boolean;
  idusuario: number;
  idacceso: number;
  idempleado: number;
  listaIdHijos;
  public calificaciones = UtilesSeguimientos.calificaciones;
  observaciones: TipoDeObservacion[]; // arreglo con los tipos de observaciones
  empleados: EmpleadoParaSeguir[];  // arreglo con los empleados que se pueden observar
  idsector: any;
  spinner = false;

  constructor(
    private mensajeriaService: MensajeriaService,
    private empleadosService: EmpleadosService,
    private dialog: MatDialog,
    private fb: FormBuilder,
    public dialogRef: MatDialogRef<NuevoSeguimientoComponent>,
    @Inject(MAT_DIALOG_DATA) data
  ) {
    if (data.idempleado > 0) {
      this.seguimientoDeSala = false;
      this.idsector = data.idsector;
    } else {
      this.seguimientoDeSala = true;
    }
    this.idacceso = data.idacceso;
    this.idempleado = data.idempleado;
    this.idusuario = data.idusuario;
    this.listaIdHijos = data.listaIdHijos;

    this.form = fb.group({
      seguimientoCtrl: [false, []],
      calificacionesCtrl: [false, [Validators.required]],
      observacionCtrl: [false, [Validators.required]],
      empleadoAobservarCtrl: [false, [Validators.required]],
      detalleCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]]
      /* releasedAt: [(new Date()).toISOString(), Validators.required], */
    });
  }

  ngOnInit() {
    this.obtenerDetalles();
  }

  save() {
    const message2 = `Está seguro de que quiere guardar el seguimiento?`;
    const dialogData2 = new ConfirmDialogModel('Confirmar', message2);
    const dialogRef2 = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData2});
    dialogRef2.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;
        const data = {
          idusuario: this.idusuario,
          tipoobservacion: this.f.observacionCtrl.value.id,
          calificacion: this.f.calificacionesCtrl.value.valor,
          sector: this.idsector,
          idempleado: this.f.empleadoAobservarCtrl.value.id,
          detalle: this.f.detalleCtrl.value
        };
        if (this.seguimientoDeSala) {
          data.sector = this.f.empleadoAobservarCtrl.value.sector;
          data.idempleado = this.f.empleadoAobservarCtrl.value.id;
        } else {
          data.sector = this.idsector;
          data.idempleado = this.idempleado;
        }

        const formData = new FormData();
        formData.append('data', JSON.stringify(data));
        this.empleadosService.postNuevoSeguimiento(formData).subscribe(
          (res) => {
            this.spinner = false;
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              this.dialogRef.close('ok');
            } else {
              /*const message3 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData3 = new PopupInformativoModel('Error', message3, 'error');
              const dialogRef3 = this.dialog.open(PopupInformativoComponent, {data: dialogData3});*/
            }
          }
        );
      }
    });
  }

  close() {
    this.dialogRef.close();
  }

  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessage() {
    return this.form.get('detalleCtrl').hasError('required') ? 'Este campo es obligatorio' :
        this.form.get('detalleCtrl').hasError('minlength') ? 'El texto ingresado es corto' :
            '';
  }

  obtenerDetalles() {
    this.mensajeriaService.getDatosParaSeguimiento(this.listaIdHijos, this.idacceso).subscribe(
      (res) => {
        /* tslint:disable:no-string-literal */
        if (res['status'] === 200) {
          this.empleados = res['body'][0];
          this.observaciones = res['body'][1];
        } else {
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
        }
        /* tslint:enable:no-string-literal */
      }
    );
  }

  get deshabilitado() {
    if (this.form.invalid) {
      return true;
    } else {
      return false;
    }
  }

}
