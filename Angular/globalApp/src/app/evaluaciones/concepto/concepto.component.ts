import { Component, OnInit, Input, Output } from '@angular/core';
import { Validators, FormBuilder } from '@angular/forms';
import { MatDialog } from '@angular/material';
import { AuthService } from 'src/app/auth/auth.service';
import { Router, ActivatedRoute } from '@angular/router';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { formatDate } from '@angular/common';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { EventEmitter } from '@angular/core';
import { Observable, of } from 'rxjs';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { EvaluacionesListado } from '../comunes/evaluacion';

@Component({
  selector: 'app-concepto',
  templateUrl: './concepto.component.html',
  styleUrls: ['./concepto.component.css']
})
export class ConceptoComponent implements OnInit {

  @Input() public evaluacion: EvaluacionesListado;
  @Input() public devolucion;
  @Input() public evaluado;
  @Output() guardado = new EventEmitter();
  public spinner = false;
  public form;
  private idusuario: number;
  @Input() public devoluciones;
  puedoDesactivar: boolean;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private evaluacionesService: EvaluacionesService,
    private authService: AuthService,
    private dialog: MatDialog,
    private fb: FormBuilder
  ) {
    this.idusuario = this.authService.currentUserValue.id;
  }

  ngOnInit() {
    this.spinner = true;

    if (this.devolucion == null) {
      this.devolucion = {
        positivo: '',
        negativo: '',
        id: 0,
        entregada: 'NO',
        rtaempleado: ''
      };
      this.obtenerBaseDevolucion();
    }

    this.form = this.fb.group({
      positivoCtrl: [this.devolucion.positivo, [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]],
      negativoCtrl: [this.devolucion.negativo, [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]],
      entregadaCtrl: [this.devolucion.entregada, []],
      respuestaCtrl: [this.devolucion.rtaempleado, [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]],
    });

    this.inicializarFormulario();
    this.spinner = false;
  }

  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessageMinMaxRequired(control: string) {
    return this.form.get(control).hasError('required') ? 'Este campo es obligatorio' :
        this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }

  inicializarFormulario() {
    // inicializo los checkboxs
    if (this.devolucion.entregada === 'SI') {
      this.f.entregadaCtrl.setValue(true);
    } else {
      this.f.entregadaCtrl.setValue(false);
    }
  }

  obtenerBaseDevolucion() {
    // si no existe la devolucion creo una base previa con las devoluciones actuales
    for (const dev of this.devoluciones) {
      this.devolucion.positivo += dev.positivo;
      this.devolucion.negativo += dev.negativo;
    }
  }

  test() {
    console.log('form: %o', (!this.form.dirty || !this.form.touched));
    console.log('invalid: %o', this.form.invalid);
    console.log('entregada: %o', (this.f.entregadaCtrl.value === true && this.f.respuestaCtrl.value === ''));
  }

  volver() {
    this.guardado.emit(this.evaluacion.id);
    this.router.navigate(['/evaluaciones/' + this.evaluacion.id + '/' + this.evaluacion.titulo + '/comparacion']);
  }

  deshabilitar() {
    // tslint:disable-next-line: max-line-length
    if ((!this.form.dirty || !this.form.touched) && this.form.invalid || (this.f.entregadaCtrl.value === true && this.f.respuestaCtrl.value === '') || (this.f.entregadaCtrl.value === false && this.f.respuestaCtrl.value !== '')) {
      return true;
    } else {
      return false;
    }
  }

  guardar() {
    // acomodo el valor del checkbox
    if (this.f.entregadaCtrl.value === false) {
      this.f.entregadaCtrl.setValue('NO');
    } else {
      this.f.entregadaCtrl.setValue('SI');
    }

    const message = `Está seguro de que desea guardar los cambios?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;

        const data = {
          idevaluacion: this.evaluacion.id,
          idsupervisor: this.idusuario,
          idevaluado: this.evaluado.empleadoid,
          positivo: this.f.positivoCtrl.value,
          negativo: this.f.negativoCtrl.value,
          iddevolucion: this.devolucion.id,
          rtaEmpleado: this.f.respuestaCtrl.value,
          entregada: this.f.entregadaCtrl.value
        };

        const formData = new FormData();
        formData.append('data', JSON.stringify(data));

        this.evaluacionesService.postDevolucion(formData).subscribe(
          (res) => {
            // tslint:disable: no-string-literal
            // console.log('respuesta: %o', res);
            this.spinner = false;
            if (res['status'] === 200) {
              this.puedoDesactivar = true;
              this.guardado.emit(this.evaluacion.id);
              this.router.navigate(['/evaluaciones/' + this.evaluacion.id + '/' + this.evaluacion.titulo + '/comparacion']);
            } else {
             /* const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});*/
            }
            // tslint:disable: no-string-literal
          }
        );
      }
    });
  }

  canDeactivate(): Observable<boolean> | boolean {
    // console.log('misArchivos.length: %o', this.misArchivos.length);
    // tslint:disable-next-line: max-line-length
    if (this.form.dirty || this.form.touched) {
      /* const message = `Está seguro de que quiere descartar los cambios?`;
      const dialogData = new ConfirmDialogModel('Confirmar', message);
      const dialogRef = this.dialog.open(DialogoConfirmacionComponent, dialogConfig);
      dialogRef.afterClosed().subscribe(result => {
        return of(result);
      }); */
      // TODO: mejorar y usar un modal
      // TODO: ver como lo implemento al ser un tag
      const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
      return of(confirmation);
    }
    return true;
  }

}
