import { Component, OnInit, Inject, ViewChild, ElementRef } from '@angular/core';
import { MAT_DIALOG_DATA, MatAutocomplete, MatDialog, MatDialogRef,
  MatChipInputEvent, MatAutocompleteSelectedEvent } from '@angular/material';
import { FormControl, Validators, FormBuilder } from '@angular/forms';
import { ENTER, COMMA } from '@angular/cdk/keycodes';
import { Observable } from 'rxjs';
import { Area } from 'src/app/mensajeria/comunes/correo';
import { MensajeriaService } from 'src/app/mensajeria/servicios/mensajeria.service';
import { RecursosService } from '../servicios/recursos.service';
import { AuthService } from 'src/app/auth/auth.service';
import { startWith, map } from 'rxjs/operators';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';

@Component({
  selector: 'app-edicion',
  templateUrl: './edicion.component.html',
  styleUrls: ['./edicion.component.css']
})
export class EdicionComponent implements OnInit {
  spinner = true;
  titulo: string;
  areas;
  comentario: string;
  tipo: string;
  direccion: string;
  id: number;
  broadcast = false;

  // esto es de los ChipsAutocomplete
  visible = true;
  selectable = true;
  removable = true;
  addOnBlur = true;
  separatorKeysCodes: number[] = [ENTER, COMMA];
  //
  // Set this to false to ensure usuarios are from todosLosUsuarios list only.
  // Set this to true to also allow 'free text' usuarios.
  //
  private allowFreeTextAddUsuarios = false;
  @ViewChild('areasInput', {static: false}) areasInput: ElementRef<HTMLInputElement>;
  @ViewChild('auto2', {static: false}) matAutocomplete2: MatAutocomplete;

  // ChipsAutocomplete Area
  areasCtrl = new FormControl();
  areasFiltradas: Observable<string[]>;
  chipSelectedAreas: Area[] = [];
  todasLasAreas: Area[];

  tipos = [
    {
      descripcion: 'Link YouTube',
      valor: 'V'
    },
    {
      descripcion: 'Documento',
      valor: 'D'
    }
  ];

  archivo: File = null;  // para almacenar el archivo

  form = this.fb.group({
    // areasCtrl: [this.areasFiltradas],
    tituloCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(50)]],
    contenidoCtrl: [null, [Validators.required]],
    tipoCtrl: [null, [Validators.required]],
    archivoCtrl: [''],
    broadcastCtrl: [false],
    direccionCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(500)]], // solo para links youtube
    comentarioCtrl: ['', [Validators.minLength(2), Validators.maxLength(5000)]]
  });
  correoRestringido: number;
  idacceso: number;
  sizeError = false;

  constructor(
    private fb: FormBuilder,
    private mensajeriaService: MensajeriaService,
    private recursosService: RecursosService,
    private authService: AuthService,
    private dialog: MatDialog,
    public dialogRef: MatDialogRef<EdicionComponent>,
    @Inject(MAT_DIALOG_DATA) public data: any
  ) {
    this.id = data.id;
    this.titulo = data.titulo;
    this.areas = data.areas;
    this.comentario = data.comentario;
    this.tipo = data.tipo;
    this.direccion = data.direccion;
    this.correoRestringido = this.authService.currentUserValue.correorestringido;
    this.idacceso = this.authService.currentUserValue.idacceso;
  }

  ngOnInit() {
    // preparo las areas
    // transformo el string en un arreglo de strings y elimino los espacios es blanco del principio y final
    this.areas = this.areas.split(',');
    for (let i = 0; i < this.areas.length; i++) {
      this.areas[i] = this.areas[i].trim();
    }

    // acomodo en caso de ser la edicion de un video
    if (this.id > 0) {
      if (this.tipo === 'V') {
        this.direccion = this.direccion.replace('https://www.youtube.com/watch?v=', 'https://www.youtube.com/embed/');
        if (!this.direccion.endsWith('?rel=0')) {
          this.direccion += '&showinfo=0&iv_load_policy=3?rel=0'; // evita sugerencias de otros videos al terminar la reproduccion
        }
      }
    }

    this.obtenerTodasLasAreas();
  }

  obtenerTodasLasAreas() {
    this.mensajeriaService.getUsuariosNuevoCorreo(this.correoRestringido, this.idacceso).subscribe(
      (res) => {
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.todasLasAreas = res['body'][1] as unknown as Area[];
          this.areasFiltradas = this.areasCtrl.valueChanges.pipe(
            startWith(null),
            map(areaNombre => this.filterOnValueChange2(areaNombre)));
          this.inicializarForm();
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  inicializarForm() {
    this.f.tituloCtrl.setValue(this.titulo);
    this.f.direccionCtrl.setValue(this.direccion);
    this.f.comentarioCtrl.setValue(this.comentario);
    this.f.archivoCtrl.setValue('');
    if (this.tipo === 'V') {
      this.f.tipoCtrl.setValue(this.tipos[0]);
    } else {
      this.f.tipoCtrl.setValue(this.tipos[1]);
    }

    // inicializo el chipSelectedAreas si es una edición
    if (this.id > 0) {
      for (const area of this.areas) {
        // verifico si es nulo o undefined para ver si tengo que inicializarlo
        if (this.chipSelectedAreas == null) {
          const aux = this.todasLasAreas.find( x => x.area === area);
          this.chipSelectedAreas = [aux];
        } else {
          const aux = this.todasLasAreas.find( x => x.area === area);
          this.chipSelectedAreas.push(aux);
        }
      }
      // compruebo si es un broadcast
      if (this.chipSelectedAreas.length === this.todasLasAreas.length) {
        this.broadcast = true;
        this.f.broadcastCtrl.setValue(true);
      }
    }

    this.spinner = false;
  }

  test() {
    console.log('chips: %o', this.chipSelectedAreas);
  }

  //#region onChange
  // actuliza el tipo en función al cambio en el select
  onChangeOption(event) {
    this.tipo = event.value.valor;
  }

  // actuliza la opción de broadcast en función al cambio en el checkbox
  onChangeBroadcast(event) {
    this.broadcast = event.checked;
    // cargo todas las areas en el chipSelectedArea
    if (this.broadcast) {
      this.chipSelectedAreas = this.todasLasAreas;
    } else {
      // inicializo el chipSelectedAreas si es una edición
      if (this.id > 0) {
        this.chipSelectedAreas = [];
        for (const area of this.areas) {
          const aux = this.todasLasAreas.find( x => x.area === area);
          this.chipSelectedAreas.push(aux);
        }
      } else {
        this.chipSelectedAreas = [];
      }
    }
  }
  //#endregion onChange

  //#region archivo
  // this is used to trigger the input
  openInput() {
    // your can use ElementRef for this later
    document.getElementById('fileInput').click();
  }

  adjuntarArchivo(fileInput: any) {
    const file = fileInput.target.files[0] as File;
    if (file.size < 10000000) { // 10Mb
      this.archivo = file;
      this.sizeError = false;
    } else {
      this.archivo = null;
      this.sizeError = true;
    }
  }

  removerArchivo(fileInput: any): void {
    this.archivo = null;
  }
  //#endregion archivo

  //#region utilidadesForm
  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessageMinMaxRequired(control: string) {
    return this.form.get(control).hasError('required') ? 'Este campo es obligatorio' :
        this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }

  getErrorMessageMinMax(control: string) {
    return this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }
  //#endregion utilidadesForm

  // TODO: disabled

  save() {
    const message2 = `Está seguro de que desea guardar los cambios?`;
    const dialogData2 = new ConfirmDialogModel('Confirmar', message2);
    const dialogRef2 = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData2});

    dialogRef2.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;
        //#region preparacion de los datos
        // tslint:disable-next-line: max-line-length
        const data = {
          idrecurso: this.id,
          titulo: this.f.tituloCtrl.value,
          comentario: this.f.comentarioCtrl.value,
          direccion: this.f.direccionCtrl.value,
          tipo: this.tipo,
          destinos: this.chipSelectedAreas
        };
        //#endregion

        const formData = new FormData();
        formData.append('data', JSON.stringify(data));
        if (this.archivo != null) {
          formData.append('archivo', this.archivo);
        }

        this.recursosService.postRecurso(formData).subscribe(
          (res) => {
            // tslint:disable: no-string-literal
            // console.log('respuesta: %o', res['body']);
            this.spinner = false;
            if (res['status'] === 200) {
              this.dialogRef.close('ok');
            } else {
             /* const message3 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData3 = new PopupInformativoModel('Error', message3, 'danger');
              const dialogRef3 = this.dialog.open(PopupInformativoComponent, {data: dialogData3});*/
            }
            // tslint:disable: no-string-literal
          }
        );
      }
    });
  }

  get deshabilitado() {
    if (this.id > 0) {
      if (this.f.tituloCtrl.invalid || this.f.comentarioCtrl.invalid || this.chipSelectedAreas.length <= 0) {
        return true;
      } else {
        return false;
      }
    } else {
      if (this.form.dirty || this.form.touched) {
        if (this.f.tituloCtrl.invalid || this.f.comentarioCtrl.invalid || this.chipSelectedAreas.length <= 0
          || (this.tipo === 'D' && this.archivo == null) || (this.tipo === 'V' && this.f.direccionCtrl.invalid) ) {
          return true;
        } else {
          return false;
        }
      } else {
        return true;
      }
    }
  }
  // TODO: canDeactive

  //#region chipsAutocomplete
  // Areas
  public addArea(event: MatChipInputEvent): void {
    if (!this.allowFreeTextAddUsuarios) {
      // only allowed to select from the filtered autocomplete list
      return;
    }

    //
    // Only add when MatAutocomplete is not open
    // To make sure this does not conflict with OptionSelected Event
    //
    if (this.matAutocomplete2.isOpen) {
      return;
    }

     // Add our usuario
    const value = event.value;
    if ((value || '').trim()) {
      this.selectAreaByName(value.trim());
    }

    this.resetInputs2();
  }

  public removeArea(area: Area): void {
    const index = this.chipSelectedAreas.indexOf(area);
    if (index >= 0) {
      this.chipSelectedAreas.splice(index, 1);
      this.resetInputs2();
    }
  }

  public areaSelected(event: MatAutocompleteSelectedEvent): void {
    this.selectAreaByName(event.option.value);
    this.resetInputs2();
  }

  private resetInputs2() {
    // clear input element
    this.areasInput.nativeElement.value = '';
    // clear control value and trigger areasCtrl.valueChanges event
    this.areasCtrl.setValue(null);
  }

  // Compute a new autocomplete list each time control value changes
  private filterOnValueChange2(areaNombre: string | null): string[] {
    let result: string[] = [];
    //
    // Remove the areas we have already selected from all areas to
    // get a starting point for the autocomplete list.
    //
    const todasLasAreasMenosLasSeleccionadas = this.todasLasAreas.filter(area => this.chipSelectedAreas.indexOf(area) < 0);
    if (areaNombre) {
      result = this.filterArea(todasLasAreasMenosLasSeleccionadas, areaNombre);
    } else {
      result = todasLasAreasMenosLasSeleccionadas.map(area => area.area);
    }
    return result;
  }

  private filterArea(areaList: Area[], areaNombre: string): string[] {
    let filteredAreaList: Area[] = [];
    const filterValue = areaNombre.toLowerCase();
    const areasMatchingAreasName = areaList.filter(area => area.area.toLowerCase().indexOf(filterValue) === 0);
    if (areasMatchingAreasName.length || this.allowFreeTextAddUsuarios) {
      //
      // either the area name matched some autocomplete options
      // or the name didn't match but we're allowing
      // non-autocomplete area names to be entered
      //
      filteredAreaList = areasMatchingAreasName;
    } else {
      //
      // the area name didn't match the autocomplete list
      // and we're only allowing area to be selected from the list
      // so we show the whjole list
      //
      filteredAreaList = areaList;
    }
    //
    // Convert filtered list of area objects to list of area
    // name strings and return it
    //
    return filteredAreaList.map(area => area.area);
  }

  private selectAreaByName(areaName) {
    const foundArea = this.todasLasAreas.filter(area => area.area === areaName);
    if (foundArea.length) {
      //
      // We found the area name in the todosLosAreas list
      //
      this.chipSelectedAreas.push(foundArea[0]);
    } else {
      //
      // Create a new area, assigning a new higher id
      // This is the use case when allowFreeTextAddEngineer is true
      const highestAreaId = Math.max(...this.chipSelectedAreas.map(area => area.id), 0);
      this.chipSelectedAreas.push({ area: areaName, id: highestAreaId + 1 });
    }
  }
  //#endregion chipsAutocomplete

  close() {
    this.dialogRef.close();
  }

}
