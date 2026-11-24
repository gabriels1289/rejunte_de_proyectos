import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { FormControl, FormBuilder, Validators } from '@angular/forms';
import { Observable } from 'rxjs';
import { Usuario, Area } from 'src/app/mensajeria/comunes/correo';
import { MatAutocomplete, MatDialog, MatChipInputEvent, MatAutocompleteSelectedEvent, PageEvent, MatDialogConfig } from '@angular/material';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { Router, ActivatedRoute } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { ENTER, COMMA } from '@angular/cdk/keycodes';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { map, startWith } from 'rxjs/operators';
import { Utilidades } from 'src/app/utilidades/utilidades';
import { CrearCompComponent } from '../crear-comp/crear-comp.component';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';

@Component({
  selector: 'app-crear-evaluacion',
  templateUrl: './crear-evaluacion.component.html',
  styleUrls: ['./crear-evaluacion.component.css']
})
export class CrearEvaluacionComponent implements OnInit {

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  // MatPaginator Inputs
  pageSize = 10;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  // esto es de los ChipsAutocomplete
  visible = true;
  selectable = true;
  removable = true;
  addOnBlur = true;
  separatorKeysCodes: number[] = [ENTER, COMMA];
  // ChipsAutocomplete Usuario
  usuariosCtrl = new FormControl();
  usuariosFiltrados: Observable<string[]>;
  chipSelectedUsuarios: Usuario[] = [];
  todosLosUsuarios: Usuario[];
  // ChipsAutocomplete Area
  areasCtrl = new FormControl();
  areasFiltradas: Observable<string[]>;
  chipSelectedAreas: Area[] = [];
  todasLasAreas: Area[];

  color = 'primary';  // color para el slide toogle
  //
  // Set this to false to ensure usuarios are from todosLosUsuarios list only.
  // Set this to true to also allow 'free text' usuarios.
  //
  private allowFreeTextAddUsuarios = false;

  @ViewChild('usuariosInput', {static: false}) usuariosInput: ElementRef<HTMLInputElement>;
  @ViewChild('auto', {static: false}) matAutocomplete: MatAutocomplete;
  @ViewChild('areasInput', {static: false}) areasInput: ElementRef<HTMLInputElement>;
  @ViewChild('auto2', {static: false}) matAutocomplete2: MatAutocomplete;

  compSeleccionada: any;
  competencias: any;
  comportamientos: any;
  idusuario: number;
  idacceso: number;
  listaIdHijos: any;

  form = this.fb.group({
    nombreCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(200)]]
  });

  usarPlantilla: boolean;
  tituloGrupo = [];
  modelo = [];
  plantillas: any;
  plantillaSel: any;
  plantillaModificada = false;
  destinos: any[];
  evaluadores: any[];

  constructor(
    private fb: FormBuilder,
    private evaluacionesService: EvaluacionesService,
    private router: Router,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    public dialog: MatDialog
  ) {
    this.idusuario = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.listaIdHijos = this.authService.currentUserValue.listaIdHijos;
  }

  ngOnInit() {
    this.obtenerDatosParaNuevaEvaluacion();
  }

  obtenerDatosParaNuevaEvaluacion() {
    this.spinner = true;
    const listaPermitidos = this.idacceso.toString() + ',' + this.listaIdHijos.toString();
    this.evaluacionesService.getDatosParaNuevaEvaluacion(this.idusuario, listaPermitidos)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          // areas y supervisores
          this.todosLosUsuarios = res['body'][0][0] as unknown as Usuario[];
          this.usuariosFiltrados = this.usuariosCtrl.valueChanges.pipe(
            startWith(null),
            map(usuarioNombre => this.filterOnValueChange(usuarioNombre)));
          this.todasLasAreas = res['body'][0][1] as unknown as Area[];
          this.areasFiltradas = this.areasCtrl.valueChanges.pipe(
            startWith(null),
            map(areaNombre => this.filterOnValueChange2(areaNombre)));

          // comptenecias y comportamientos
          this.competencias = res['body'][1][0];
          this.comportamientos = res['body'][1][1];
          for (const comportamiento of this.comportamientos) {
            comportamiento.check = false;
          }
          this.compSeleccionada = this.competencias[0];

          // lista de plantillas
          this.plantillas = res['body'][2];
          this.plantillaSel = this.plantillas[0];
          this.spinner = false;
        } else if (res['status'] === 204) {
          const message = `No hay competencias o comportamientos`;
          const dialogData = new PopupInformativoModel('Aviso', message, 'primary');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      });
  }

  // convenience getter for easy access to form fields
  get f() { return this.form.controls; }

  getErrorMessageMinMaxRequired(control: string) {
    return this.form.get(control).hasError('required') ? 'Este campo es obligatorio' :
        this.form.get(control).hasError('minlength') ? 'El texto ingresado es corto' :
        this.form.get(control).hasError('maxlength') ? 'El texto ingresado es largo' :
            '';
  }

  cargarPlantilla() {
    this.evaluacionesService.getPlantilla(this.plantillaSel.modelo)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.modelo = res['body'][0];
          this.tituloGrupo = res['body'][1];
        } else if (res['status'] === 204) {
          const message = `No se encontró la plantilla`;
          const dialogData = new PopupInformativoModel('Aviso', message, 'primary');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          this.spinner = false;
        } else {
          const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      });
  }

  actualizarPlantilla() {
    if (this.usarPlantilla === true) {
      this.cargarPlantilla();
      this.plantillaModificada = false;
    } else {
      // si desactiva la plantilla la quito
      this.modelo = [];
      this.tituloGrupo = [];
    }
  }

  quitarDeLaLista(indice) {
    if (this.usarPlantilla === true) {
      this.plantillaModificada = true;
    }
    this.modelo.splice(indice, 1);
    this.tituloGrupo = [];
    for (const mod of this.modelo) {
      const temp = this.modelo.filter(x => x.idcompe === mod.idcompe);
      for (const x of temp) {
        this.tituloGrupo.push({id: x.idcompe, competencia: x.competencia});
      }
    }
    this.tituloGrupo = Utilidades.eliminarDuplicados(this.tituloGrupo);
  }

  agregar() {
    if (this.usarPlantilla === true) {
      this.plantillaModificada = true;
    }
    for (const comportamiento of this.comportamientos) {
      if (comportamiento.check === true) {
        let repetido = false;
        for (const mod of this.modelo) {
          if ((comportamiento.id === mod.idcompor) && (this.compSeleccionada.id === mod.idcompe)) {
            repetido = true;
          }
        }
        if (repetido === false) {
          // tslint:disable-next-line: max-line-length
          this.modelo.push({idcompe: this.compSeleccionada.id, competencia: this.compSeleccionada.competencia, idcompor: comportamiento.id, comportamiento: comportamiento.comportamiento});
        }
      }
    }
    for (const comportamiento of this.comportamientos) {
      if (comportamiento.check === true) {
        comportamiento.check = false;
      }
    }
    for (const mod of this.modelo) {
      const temp = this.modelo.filter(x => x.idcompe === mod.idcompe);
      for (const x of temp) {
        this.tituloGrupo.push({id: x.idcompe, competencia: x.competencia});
      }
    }
    this.tituloGrupo = Utilidades.eliminarDuplicados(this.tituloGrupo);
  }

  cancelar() {
    this.router.navigate(['/evaluaciones']);
  }

  crearComp() {
    const dialogConfig = new MatDialogConfig();

    dialogConfig.disableClose = true;
    dialogConfig.autoFocus = true;

    dialogConfig.data = {
        idusuario: this.idusuario
    };

    const dialogRef = this.dialog.open(CrearCompComponent, dialogConfig);

    dialogRef.afterClosed().subscribe(
      (res) => {
        if (res === 'ok') {
          this.obtenerDatosParaNuevaEvaluacion();
        }
      }
    );
  }

  test() {
    console.log('Evaluadores: %o', this.chipSelectedUsuarios);
    console.log('Areas: %o', this.chipSelectedAreas);
  }

  guardar() {
    const message = `Está seguro de que quiere guardar los cambios?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.spinner = true;
        this.destinos = Utilidades.eliminarDuplicados(this.chipSelectedAreas);
        this.evaluadores = Utilidades.eliminarDuplicados(this.chipSelectedUsuarios);

        let auxUsarPlantilla = false;
        if (this.usarPlantilla === true && this.plantillaModificada === false) {
          auxUsarPlantilla = true;
        } else {
          auxUsarPlantilla = false;
        }

        const data = {
          idusuario: this.idusuario,
          destinos: this.destinos,
          nombreevaluacion: this.f.nombreCtrl.value,
          modelo: this.modelo,
          evaluadores: this.evaluadores,
          usarplantilla: auxUsarPlantilla,
          numeroplantilla: 0
        };

        // si usa una plantilla agrego a los datos a enviar el número
        if (auxUsarPlantilla === true) {
          data.numeroplantilla = this.plantillaSel.modelo;
        }

        const formData = new FormData();
        formData.append('data', JSON.stringify(data));

        this.evaluacionesService.postNuevaEvaluacion(formData)
        .subscribe(
          (res) => {
            console.log(res);
            // tslint:disable: no-string-literal
            if (res['status'] === 200) {
              this.router.navigate(['/evaluaciones']);
              this.spinner = false;
            } else {
              const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});
              this.spinner = false;
            }
            // tslint:enable: no-string-literal
          });
      }
    });
  }

  //#region paginador
  // función para setear el tamaño de la paginación
  setPageSizeOptions(setPageSizeOptionsInput: string) {
    this.pageSizeOptions = setPageSizeOptionsInput.split(',').map(str => +str);
  }

  public handlePage(e: PageEvent): PageEvent {
    this.pageSize = e.pageSize;
    this.pageIndex = e.pageIndex;
    return e;
  }
  //#endregion

  //#region chipsAutocomplete
  public addUsuario(event: MatChipInputEvent): void {
    if (!this.allowFreeTextAddUsuarios) {
      // only allowed to select from the filtered autocomplete list
      // console.log('allowFreeTextAddUsuarios is false');
      return;
    }

    //
    // Only add when MatAutocomplete is not open
    // To make sure this does not conflict with OptionSelected Event
    //
    if (this.matAutocomplete.isOpen) {
      return;
    }

     // Add our usuario
    const value = event.value;
    if ((value || '').trim()) {
      this.selectUsuarioByName(value.trim());
    }

    this.resetInputs();
  }

  public removeUsuario(usuario: Usuario): void {
    const index = this.chipSelectedUsuarios.indexOf(usuario);
    if (index >= 0) {
      this.chipSelectedUsuarios.splice(index, 1);
      this.resetInputs();
    }
  }

  public usuarioSelected(event: MatAutocompleteSelectedEvent): void {
    this.selectUsuarioByName(event.option.value);
    this.resetInputs();
  }

  private resetInputs() {
    // clear input element
    this.usuariosInput.nativeElement.value = '';
    // clear control value and trigger usuariosCtrl.valueChanges event
    this.usuariosCtrl.setValue(null);
  }

  // Compute a new autocomplete list each time control value changes
  private filterOnValueChange(usuarioNombre: string | null): string[] {
    let result: string[] = [];
    //
    // Remove the usuarios we have already selected from all ususarios to
    // get a starting point for the autocomplete list.
    //
    const todosLosUsuariosMenosLosSeleccionados = this.todosLosUsuarios.filter(usuario => this.chipSelectedUsuarios.indexOf(usuario) < 0);
    if (usuarioNombre) {
      result = this.filterUsuario(todosLosUsuariosMenosLosSeleccionados, usuarioNombre);
    } else {
      result = todosLosUsuariosMenosLosSeleccionados.map(usuario => usuario.usuario);
    }
    return result;
  }

  private filterUsuario(usuarioList: Usuario[], usuarioNombre: string): string[] {
    let filteredUsuarioList: Usuario[] = [];
    const filterValue = usuarioNombre.toLowerCase();
    const usuariosMatchingUsuariosName = usuarioList.filter(usuario => usuario.usuario.toLowerCase().indexOf(filterValue) === 0);
    if (usuariosMatchingUsuariosName.length || this.allowFreeTextAddUsuarios) {
      //
      // either the usuario name matched some autocomplete options
      // or the name didn't match but we're allowing
      // non-autocomplete usuario names to be entered
      //
      filteredUsuarioList = usuariosMatchingUsuariosName;
    } else {
      //
      // the usuario name didn't match the autocomplete list
      // and we're only allowing usuario to be selected from the list
      // so we show the whjole list
      //
      filteredUsuarioList = usuarioList;
    }
    //
    // Convert filtered list of usuario objects to list of usuario
    // name strings and return it
    //
    return filteredUsuarioList.map(usuario => usuario.usuario);
  }

  private selectUsuarioByName(usuarioName) {
    const foundUsuario = this.todosLosUsuarios.filter(usuario => usuario.usuario === usuarioName);
    if (foundUsuario.length) {
      //
      // We found the usuario name in the todosLosUsuarios list
      //
      this.chipSelectedUsuarios.push(foundUsuario[0]);
    } else {
      //
      // Create a new usuario, assigning a new higher id
      // This is the use case when allowFreeTextAddEngineer is true
      //  Y un Area vacia
      const highestUsuarioId = Math.max(...this.chipSelectedUsuarios.map(usuario => usuario.id), 0);
      this.chipSelectedUsuarios.push({ usuario: usuarioName, id: highestUsuarioId + 1, area: '' });
    }
  }


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
    if (this.matAutocomplete.isOpen) {
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

  //#region funciones trackBy
  public trackByFunctionIdSup(index, item) {
    if (!item) {
      return null;
    }
    return item.idsup; // or return index
  }

  public trackByFunctionId(index, item) {
    if (!item) {
      return null;
    }
    return item.id; // or return index
  }

  public trackByFunctionIndex(index, item) {
    return index; // or return index
  }

  public trackByFunctionIdPe(index, item) {
    if (!item) {
      return null;
    }
    return item.idpe; // or return index
  }
  //#endregion
}
