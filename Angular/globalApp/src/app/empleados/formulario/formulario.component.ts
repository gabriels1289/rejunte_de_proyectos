import { Component, OnInit, Input, Output } from '@angular/core';
import { Validators, FormBuilder } from '@angular/forms';
import { MatDialog, MatDatepickerInputEvent } from '@angular/material';
import { AuthService } from 'src/app/auth/auth.service';
import { EmpleadosService } from '../servicios/empleados.service';
import { Router, ActivatedRoute } from '@angular/router';
import { Empleado, Sexo } from '../comunes/empleado';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { formatDate } from '@angular/common';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { EventEmitter } from '@angular/core';
import { Observable, of } from 'rxjs';

@Component({
  selector: 'app-formulario',
  templateUrl: './formulario.component.html',
  styleUrls: ['./formulario.component.css']
})
export class FormularioComponent implements OnInit {

  @Input() public empleado: Empleado;
  @Output() guardado = new EventEmitter();
  public spinner = false;
  public spinnerCargaDeFoto = false;
  public form;
  private idacceso: number;
  public empresas;
  public sectores;
  public obrasSociales;
  public arts;
  public sexos: Sexo[] = [
    {valor: 'M', descripcion: 'MASCULINO'},
    {valor: 'F', descripcion: 'FEMENINO'}
  ];
  fotoToUpload: File = null;
  previewUrl: string | ArrayBuffer = '';
  actualizarFoto: boolean;
  edicion = false;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private empleadosService: EmpleadosService,
    private authService: AuthService,
    private dialog: MatDialog,
    private fb: FormBuilder
  ) {
    this.idacceso = this.authService.currentUserValue.idacceso;
  }

  ngOnInit() {

    if (this.empleado === undefined) {
      this.edicion = false;
      this.empleado = new Empleado();
      this.empleado.id = 0;
      this.empleado.legajo = 0;
      this.empleado.nombre = '';
      this.empleado.apellido = '';
      this.empleado.direccion = '';
      this.empleado.localidad = 'BAHIA BLANCA';
      this.empleado.codigopostal = '8000';
      this.empleado.telefonofijo = '';
      this.empleado.telefonomovil = '';
      this.empleado.mail = '';
      this.empleado.fechanacimiento = new Date();
      this.empleado.tipodocumento = 'DNI';
      this.empleado.numerodocumento = '';
      this.empleado.cuil = '';
      this.empleado.legajoestudio = 0;
      this.empleado.preocupacional = '';
      this.empleado.preocupacionaldetalle = '';
      this.empleado.basico = 0;
      this.empleado.sexo = '';
      this.empleado.activo = 'SI';
      this.empleado.fulltime = 'SI';
      this.empleado.foto = 'img/user.jpg';
      this.empleado.idart = 1;
      this.empleado.idobrasocial = 1;
      this.empleado.idsector = 24;
      this.empleado.idempresa = 1;
    } else {
      this.edicion = true;
    }

    this.form = this.fb.group({
      fotoCtrl: [null],
      nombreCtrl: [this.empleado.nombre, [Validators.required, Validators.minLength(2), Validators.maxLength(45)]],
      apellidoCtrl: [this.empleado.apellido, [Validators.required, Validators.minLength(2), Validators.maxLength(45)]],
      sexoCtrl: [this.empleado.sexo, [Validators.required]],
      tipodocumentoCtrl: [this.empleado.tipodocumento, [Validators.required, Validators.minLength(2), Validators.maxLength(45)]],
      numerodocumentoCtrl: [this.empleado.numerodocumento, [Validators.required, Validators.minLength(2), Validators.maxLength(15)]],
      // tslint:disable-next-line: max-line-length
      fechanacimientoCtrl: [(new Date(this.empleado.fechanacimiento)).toISOString(), [Validators.required]], // TODO: ver como se validan los rangos
      localidadCtrl: [this.empleado.localidad, [Validators.required, Validators.minLength(2), Validators.maxLength(45)]],
      codigopostalCtrl: [this.empleado.codigopostal, [Validators.required, Validators.minLength(2), Validators.maxLength(10)]],
      direccionCtrl: [this.empleado.direccion, [Validators.required, Validators.minLength(2), Validators.maxLength(200)]],
      mailCtrl: [this.empleado.mail, [Validators.maxLength(200), Validators.email]],
      telefonomovilCtrl: [this.empleado.telefonomovil, [Validators.maxLength(45)]],
      telefonofijoCtrl: [this.empleado.telefonofijo, [Validators.maxLength(45)]],
      activoCtrl: [this.empleado.activo, [Validators.required]],
      legajoCtrl: [this.empleado.legajo, [Validators.required, Validators.minLength(1), Validators.maxLength(10)]],
      cuilCtrl: [this.empleado.cuil, [Validators.required, Validators.minLength(7), Validators.maxLength(15)]],
      fechacontratoCtrl: [(new Date(this.empleado.fechacontrato).toISOString), []], // TODO: ver como se validan los rangos
      fechabajaCtrl: [(new Date(this.empleado.fechabaja)).toISOString, []], // TODO: ver como se validan los rangos
      fulltimeCtrl: [this.empleado.fulltime, []],
      empresaCtrl: [this.empleado.razonsocial, [Validators.required]],
      sectorCtrl: [this.empleado.sector, [Validators.required]],
      // basicoCtrl: [this.empleado.basico, [Validators.maxLength(11)]],
      obrasocialCtrl: [this.empleado.obrasocial, [Validators.required]],
      artCtrl: [this.empleado.art, [Validators.required]],
      legajoestudioCtrl: [this.empleado.legajoestudio, [Validators.required, Validators.minLength(1), Validators.maxLength(10)]],
      preocupacionalCtrl: [this.empleado.preocupacional, [Validators.required, Validators.maxLength(10)]],
      fechaanalisisCtrl: [(new Date(this.empleado.fechaanalisis)).toISOString, []], // TODO: ver como se validan los rangos
      preocupacionaldetalleCtrl: [this.empleado.preocupacionaldetalle, [Validators.maxLength(200)]]
      // TODO: ver legajo y legajoestudio, xq no son validos
    });

    this.getDatosSelects();
  }

  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessageMinMaxRequired(control: string) {
    return this.form.get(control).hasError('required') ? 'Este campo es obligatorio' :
        this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }

  getDatosSelects() {
    this.empleadosService.getDatosSelectsFormulario(this.idacceso).subscribe(
      (res) => {
        // console.log('Respuesta: %o', res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.empresas = res['body'][0];
          this.sectores = res['body'][1];
          this.obrasSociales = res['body'][2];
          this.arts = res['body'][3];
          this.inicializarFormulario();
        } else {
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  inicializarFormulario() {
    // inicializo los selects
    let toSelect;
    toSelect = this.sexos.find(x => x.valor === this.empleado.sexo);
    this.f.sexoCtrl.setValue(toSelect);
    toSelect = this.empresas.find(x => x.id === this.empleado.idempresa);
    this.f.empresaCtrl.setValue(toSelect);
    toSelect = this.sectores.find(x => x.id === this.empleado.idsector);
    this.f.sectorCtrl.setValue(toSelect);
    toSelect = this.obrasSociales.find(x => x.id === this.empleado.idobrasocial);
    this.f.obrasocialCtrl.setValue(toSelect);
    toSelect = this.arts.find(x => x.id === this.empleado.idart);
    this.f.artCtrl.setValue(toSelect);

    // inicializo los checkboxs
    if (this.empleado.activo === 'SI') {
      this.f.activoCtrl.setValue(true);
    } else {
      this.f.activoCtrl.setValue(false);
    }
    if (this.empleado.fulltime === 'SI') {
      this.f.fulltimeCtrl.setValue(true);
    } else {
      this.f.fulltimeCtrl.setValue(false);
    }

    // TODO: ver si implemento el Touch UI mode en los datepiker
  }

  // this is used to trigger the input
  openInput() {
    // your can use ElementRef for this later
    document.getElementById('fileInput').click();
  }

  adjuntarFoto(fileInput: any) {
    this.fotoToUpload = fileInput.target.files[0] as File;
    // TODO: verificar tamaño de la imagen y hacer un spinner mientras renderiza la previsualizacion
    this.preview();
  }

  preview() {
    // Show preview
    const mimeType = this.fotoToUpload.type;
    if (mimeType.match(/image\/*/) == null) {
      return;
    }
    this.spinnerCargaDeFoto = true;

    const reader = new FileReader();
    reader.readAsDataURL(this.fotoToUpload);
    reader.onload = (evento) => {
      this.previewUrl = reader.result;
      this.spinnerCargaDeFoto = false;
    };
  }

  cancelarFoto() {
    this.fotoToUpload = null;
    this.previewUrl = '';
  }

  deshabilitar(): boolean {
    if ((this.form.pristine || this.form.untouched) && this.previewUrl === '') {
      return true;
    } else {
      // tslint:disable: max-line-length
      if (this.f.legajoCtrl.invalid || this.f.apellidoCtrl.invalid || this.f.nombreCtrl.invalid || this.f.direccionCtrl.invalid || this.f.localidadCtrl.invalid ||
        this.f.codigopostalCtrl.invalid || this.f.telefonofijoCtrl.invalid || this.f.telefonomovilCtrl.invalid || this.f.mailCtrl.invalid ||
        this.f.fechanacimientoCtrl.invalid || this.f.tipodocumentoCtrl.invalid || this.f.numerodocumentoCtrl.invalid || this.f.cuilCtrl.invalid ||
        this.f.legajoestudioCtrl.invalid || this.f.preocupacionalCtrl.invalid || this.f.preocupacionaldetalleCtrl.invalid || // this.f.basicoCtrl.invalid ||
        this.f.sexoCtrl.invalid || this.f.activoCtrl.invalid || this.f.fulltimeCtrl.invalid || this.f.artCtrl.invalid || this.f.obrasocialCtrl.invalid ||
        this.f.sectorCtrl.invalid || this.f.empresaCtrl.invalid) {
        // tslint:enable: max-line-length
        return true;
      } else {
        return false;
      }
    }
  }

  test() {
    if (this.f.legajoCtrl.invalid) {
      console.log('legajoCtrl');
    }
    if (this.f.apellidoCtrl.invalid) {
      console.log('apellidoCtrl');
    }
    if (this.f.nombreCtrl.invalid) {
      console.log('nombreCtrl');
    }
    if (this.f.direccionCtrl.invalid) {
      console.log('direccionCtrl');
    }
    if (this.f.localidadCtrl.invalid) {
      console.log('localidadCtrl');
    }
    if (this.f.codigopostalCtrl.invalid) {
      console.log('codigopostalCtrl');
    }
    if (this.f.telefonofijoCtrl.invalid) {
      console.log('telefonofijoCtrl');
    }
    if (this.f.telefonomovilCtrl.invalid) {
      console.log('telefonomovilCtrl');
    }
    if (this.f.mailCtrl.invalid) {
      console.log('mailCtrl');
    }
    if (this.f.fechanacimientoCtrl.invalid) {
      console.log('fechanacimientoCtrl');
    }
    if (this.f.tipodocumentoCtrl.invalid) {
      console.log('tipodocumentoCtrl');
    }
    if (this.f.numerodocumentoCtrl.invalid) {
      console.log('numerodocumentoCtrl');
    }
    if (this.f.cuilCtrl.invalid) {
      console.log('cuilCtrl');
    }
    if (this.f.legajoestudioCtrl.invalid) {
      console.log('legajoestudioCtrl');
    }
    if (this.f.preocupacionalCtrl.invalid) {
      console.log('preocupacionalCtrl');
    }
    if (this.f.preocupacionaldetalleCtrl.invalid) {
      console.log('preocupacionaldetalleCtrl');
    }
    if (this.f.sexoCtrl.invalid) {
      console.log('sexoCtrl');
    }
    if (this.f.activoCtrl.invalid) {
      console.log('activoCtrl');
    }
    if (this.f.fulltimeCtrl.invalid) {
      console.log('fulltimeCtrl');
    }
    if (this.f.artCtrl.invalid) {
      console.log('artCtrl');
    }
    if (this.f.obrasocialCtrl.invalid) {
      console.log('obrasocialCtrl');
    }
    if (this.f.sectorCtrl.invalid) {
      console.log('sectorCtrl');
    }
    if (this.f.empresaCtrl.invalid) {
      console.log('empresaCtrl');
    }
  }

  enviar() {
    const message = `Está seguro de que desea guardar los cambios?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;
        //#region preparacion de los datos
        // cambio el valor de los checkbox para coincidir con la DB
        if ( this.f.activoCtrl.value === true) {
          this.f.activoCtrl.setValue('SI');
        } else {
          this.f.activoCtrl.setValue('NO');
        }
        if ( this.f.fulltimeCtrl.value === true) {
          this.f.fulltimeCtrl.setValue('SI');
        } else {
          this.f.fulltimeCtrl.setValue('NO');
        }
        // formateo las fechas que no son obligatorias
        // tslint:disable-next-line: max-line-length
        if (typeof(this.f.fechacontratoCtrl.value) === 'function' || this.f.fechacontratoCtrl.value === null || this.f.fechacontratoCtrl.value === '') {
          this.f.fechacontratoCtrl.setValue('0000-00-00');
        } else {
          this.f.fechacontratoCtrl.setValue(formatDate(this.f.fechacontratoCtrl.value, 'yyyy-MM-dd', 'es-AR'));
        }
        // tslint:disable-next-line: max-line-length
        if (typeof(this.f.fechaanalisisCtrl.value) === 'function' || this.f.fechaanalisisCtrl.value === null || this.f.fechaanalisisCtrl.value === '') {
          this.f.fechaanalisisCtrl.setValue('0000-00-00');
        } else {
          this.f.fechaanalisisCtrl.setValue(formatDate(this.f.fechaanalisisCtrl.value, 'yyyy-MM-dd', 'es-AR'));
        }
        if (typeof(this.f.fechabajaCtrl.value) === 'function' || this.f.fechabajaCtrl.value === null || this.f.fechabajaCtrl.value === '') {
          this.f.fechabajaCtrl.setValue('0000-00-00');
        } else {
          this.f.fechabajaCtrl.setValue(formatDate(this.f.fechabajaCtrl.value, 'yyyy-MM-dd', 'es-AR'));
        }

        // flag para saber si tengo que cambiar la foto
        if (this.fotoToUpload !== null && this.previewUrl !== '') {
          this.actualizarFoto = true;
        } else {
          this.actualizarFoto = false;
        }

        // si la url de la foto de empleado es 'img/user.jpg' (imagen por defecto) para evitar errores la actualizo a null
        if (this.empleado.foto === 'img/user.jpg') {
          this.empleado.foto = '';
        }

        const data = {
          idempleado: this.empleado.id,
          legajo: this.f.legajoCtrl.value,
          apellido: this.f.apellidoCtrl.value.toUpperCase(),
          nombre: this.f.nombreCtrl.value.toUpperCase(),
          direccion: this.f.direccionCtrl.value.toUpperCase(),
          localidad: this.f.localidadCtrl.value.toUpperCase(),
          codigopostal: this.f.codigopostalCtrl.value,
          telefonofijo: this.f.telefonofijoCtrl.value,
          telefonomovil: this.f.telefonomovilCtrl.value,
          mail: this.f.mailCtrl.value,
          fechanacimiento: formatDate(this.f.fechanacimientoCtrl.value, 'yyyy-MM-dd', 'es-AR'),
          // fechacontrato: formatDate(this.f.fechacontratoCtrl.value, 'yyyy-MM-dd', 'es-AR'),
          fechacontrato: this.f.fechacontratoCtrl.value,
          tipodocumento: this.f.tipodocumentoCtrl.value.toUpperCase(),
          numerodocumento: this.f.numerodocumentoCtrl.value,
          cuil: this.f.cuilCtrl.value,
          legajoestudio: this.f.legajoestudioCtrl.value,
          preocupacional: this.f.preocupacionalCtrl.value,
          preocupacionaldetalle: this.f.preocupacionaldetalleCtrl.value.toUpperCase(),
          // fechaanalisis: formatDate(this.f.fechaanalisisCtrl.value, 'yyyy-MM-dd', 'es-AR'),
          fechaanalisis: this.f.fechaanalisisCtrl.value,
          basico: 0, // this.f.basicoCtrl.value,
          sexo: this.f.sexoCtrl.value.valor,
          // fechabaja: formatDate(this.f.fechabajaCtrl.value, 'yyyy-MM-dd', 'es-AR'),
          fechabaja: this.f.fechabajaCtrl.value,
          activo: this.f.activoCtrl.value.toUpperCase(),  // true o false
          fulltime: this.f.fulltimeCtrl.value.toUpperCase(),  // true o false
          foto: this.empleado.foto,
          art: this.f.artCtrl.value.id,
          obrasocial: this.f.obrasocialCtrl.value.id,
          sector: this.f.sectorCtrl.value.id,
          empresa: this.f.empresaCtrl.value.id,
          actualizarFoto: this.actualizarFoto
        };
        //#endregion

        const formData = new FormData();
        formData.append('data', JSON.stringify(data));
        if (this.actualizarFoto === true) {
          formData.append('foto', this.fotoToUpload);
        }

        this.empleadosService.postEdicionEmpleado(formData).subscribe(
          (res) => {
            // tslint:disable: no-string-literal
            // console.log('respuesta: %o', res['body']);
            this.spinner = false;
            if (res['status'] === 200) {
              const idempleado = res['body'];
              this.guardado.emit(idempleado);
              this.router.navigate(['/empleados/' + data.idempleado + '/' + data.nombre + '/' + data.apellido + '/'
              + this.f.sectorCtrl.value.id + '/' + this.f.sectorCtrl.value.area + '/datos-personales']);
            } else {
              /*const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});*/
            }
            // tslint:disable: no-string-literal
          }
        );
      }
    });
  }

  // TODO: ver si se puede implementar cuando se usa como directiva
  canDeactivate(): Observable<boolean> | boolean {
    // console.log('misArchivos.length: %o', this.misArchivos.length);
    // tslint:disable-next-line: max-line-length
    if (this.form.dirty || this.form.touched || this.actualizarFoto) {
      /* const message = `Está seguro de que quiere descartar los cambios?`;
      const dialogData = new ConfirmDialogModel('Confirmar', message);
      const dialogRef = this.dialog.open(DialogoConfirmacionComponent, dialogConfig);
      dialogRef.afterClosed().subscribe(result => {
        return of(result);
      }); */
      // TODO: mejorar y usar un modal
      const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
      return of(confirmation);
    }
    return true;
  }

  volver() {
    /* this.router.navigate(['/empleados/' + this.empleado.id + '/' + this.empleado.nombre + '/' + this.empleado.apellido + '/'
    + this.empleado.idsector + '/' + this.empleado.sector + '/datos-personales']); */
    this.router.navigate(['/empleados/']);
  }

}
