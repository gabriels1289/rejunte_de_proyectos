import { Component, OnInit, Inject } from '@angular/core';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { MatDialog, MatDialogRef, MAT_DIALOG_DATA } from '@angular/material';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-crear-comp',
  templateUrl: './crear-comp.component.html',
  styleUrls: ['./crear-comp.component.css']
})
export class CrearCompComponent implements OnInit {

  form: FormGroup;
  seguimientoDeSala: boolean;
  idusuario: number;
  spinner = false;
  tipos = [
    {id: 0, descripcion: 'Comportamiento'},
    {id: 1, descripcion: 'Competencia'},
  ];

  constructor(
    private dialog: MatDialog,
    private fb: FormBuilder,
    public dialogRef: MatDialogRef<CrearCompComponent>,
    private evaluacionesServices: EvaluacionesService,
    @Inject(MAT_DIALOG_DATA) data
  ) {
    this.idusuario = data.idusuario;

    this.form = fb.group({
      tipoCtrl: [this.tipos, [Validators.required]],
      detalleCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]]
    });
  }

  ngOnInit() {
    this.form.controls.tipoCtrl.setValue(this.tipos[0]);
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
    if (this.form.invalid) {
      return true;
    } else {
      return false;
    }
  }

  close() {
    this.dialogRef.close();
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
          tabla: this.f.tipoCtrl.value.descripcion,
          texto: this.f.detalleCtrl.value
        };
        console.log('data: %o', data);
        const formData = new FormData();
        formData.append('data', JSON.stringify(data));
        this.evaluacionesServices.postNuevoComp(formData).subscribe(
          (res) => {
            // console.log('res: %o', res);
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

}
