import { Component, OnInit, ElementRef, ViewChild } from '@angular/core';
import { FormBuilder, Validators, FormControl } from '@angular/forms';
import { Observable, of } from 'rxjs';
import { COMMA, ENTER } from '@angular/cdk/keycodes';
import { MatAutocompleteSelectedEvent, MatAutocomplete } from '@angular/material/autocomplete';
import { MatChipInputEvent } from '@angular/material/chips';
import { map, startWith } from 'rxjs/operators';
import { MensajeriaService } from '../servicios/mensajeria.service';
import { AuthService } from '../../auth/auth.service';
import { Usuario, Area, UtilesMensajeria, Correo } from '../comunes/correo';
import { UtilesSeguimientos, EmpleadoParaSeguir, TipoDeObservacion } from '../../seguimientos/comunes/seguimiento';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from '../../modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { MatDialog } from '@angular/material';
import { Router, ActivatedRoute } from '@angular/router';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-nuevo-correo',
  templateUrl: './nuevo-correo.component.html',
  styleUrls: ['./nuevo-correo.component.css']
})
export class NuevoCorreoComponent implements OnInit {

  // la agregue por el error del compilador
  public $last;

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  // datos del usuario
  private correoRestringido;
  private idacceso;
  private listaIdHijos;
  public ruta; // url actual -> nuevo-correo / reenviar
  // datos del reenvio
  private idmensaje;
  private tabla;
  adjuntosReenvio = [];  // para mantener el arreglo de ajuntos reenviados
  // para responder un mensaje
  public detalleAresponder: Correo;
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
  destinatarios = []; // propiedad con los destinatarios finales
  permisoSeguimiento = false;  // propiedad para ver si puede o no hacer seguimientos
  // subida de archivos
  misArchivos: File[] = null;
  public calificaciones = UtilesSeguimientos.calificaciones;
  observaciones: TipoDeObservacion[]; // arreglo con los tipos de observaciones
  empleados: EmpleadoParaSeguir[];  // arreglo con los empleados que se pueden observar

  //
  // Set this to false to ensure usuarios are from todosLosUsuarios list only.
  // Set this to true to also allow 'free text' usuarios.
  //
  private allowFreeTextAddUsuarios = false;

  @ViewChild('usuariosInput', {static: false}) usuariosInput: ElementRef<HTMLInputElement>;
  @ViewChild('auto', {static: false}) matAutocomplete: MatAutocomplete;
  @ViewChild('areasInput', {static: false}) areasInput: ElementRef<HTMLInputElement>;
  @ViewChild('auto2', {static: false}) matAutocomplete2: MatAutocomplete;

  // CKEditor
  name = 'ng2-ckeditor';
  ckeConfig: any;
  mycontent = new FormControl();
  log = '';
  @ViewChild('myckeditor', {static: false}) ckeditor: any;

  puedoDesactivar = true; // para el manejo de canDeactive
  broadcast = false;


  correoForm = this.fb.group({
    // usuariosCtrl: [''],
    // areasCtrl: [this.areasFiltradas],
    broadcastCtrl: [false],
    seguimientoCtrl: [false, []],
    calificacionesCtrl: [false, [Validators.required]],
    observacionCtrl: [false, [Validators.required]],
    empleadoAobservarCtrl: [false, [Validators.required]],
    archivosCtrl: [null],
    asuntoCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(45)]],
    cuerpoCtrl: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(3000)]]
  });

  constructor(
    private fb: FormBuilder,
    private mensajeriaService: MensajeriaService,
    private router: Router,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    public dialog: MatDialog
  ) { }

  ngOnInit() {
    // datos del usuario
    this.correoRestringido = this.authService.currentUserValue.correorestringido;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.listaIdHijos = this.authService.currentUserValue.listaIdHijos;
    // datos de la ruta de reenvio en el caso de ser tal
    this.ruta = this.rutaActiva.snapshot.url.toString().split(',')[0];
    if (this.ruta === 'reenviar') {
      this.rutaActiva.queryParamMap.subscribe(
        params => {
          this.idmensaje = +params.get('idmensaje') || 0;
          this.tabla = params.get('tabla') || '';
        }
      );
      this.mensajeriaService.getDetalleReenvio(this.idmensaje, this.tabla).subscribe(
        (res) => {
          // tslint:disable: no-string-literal
          if (res['status'] === 200) {
            // tslint:disable-next-line: max-line-length
            this.correoForm.get('cuerpoCtrl').setValue('<blockquote><span style="color:#9b59b6">' + res['body'][0].cuerpo + '</span></blockquote>');
            this.correoForm.get('asuntoCtrl').setValue(res['body'][0].asunto);
            if (res['body'][0].nombre != null) {
              for (const adj of res['body']) {
                this.adjuntosReenvio.push(adj);
              }
            }
          }
          // tslint:enable: no-string-literal
        }
      );
    }

    if (this.ruta === 'responder') {
      this.idmensaje = this.rutaActiva.snapshot.params.idmensaje;
      this.mensajeriaService.getDetalleResponder(this.idmensaje).subscribe(
        (res) => {
          // tslint:disable: no-string-literal
          if (res['status'] === 200) {
            this.detalleAresponder = res['body'][0];
            this.destinatarios = res['body'][1];
            this.spinner = false;
          } else {
            /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
            const dialogData = new PopupInformativoModel('Error', message, 'danger');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
            this.spinner = false;
          }
          // tslint:enable: no-string-literal
        }
      );
    } else {
      this.mensajeriaService.getUsuariosNuevoCorreo(this.correoRestringido, this.idacceso).subscribe(
        (res) => {
          // tslint:disable: no-string-literal
          if (res['status'] === 200) {
          this.todosLosUsuarios = res['body'][0] as unknown as Usuario[];
          this.usuariosFiltrados = this.usuariosCtrl.valueChanges.pipe(
            startWith(null),
            map(usuarioNombre => this.filterOnValueChange(usuarioNombre)));
          this.todasLasAreas = res['body'][1] as unknown as Area[];
          this.areasFiltradas = this.areasCtrl.valueChanges.pipe(
            startWith(null),
            map(areaNombre => this.filterOnValueChange2(areaNombre)));
          this.spinner = false;
          } else {
           /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
            const dialogData = new PopupInformativoModel('Error', message, 'danger');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
            this.spinner = false;
          }
          // tslint:enable: no-string-literal
        }
      );
      // this.correoForm.get('cuerpo').setValue(`<p>My html content</p>`);
      this.puedeHacerSeguimiento();
      if (this.permisoSeguimiento === true) {
        this.mensajeriaService.getDatosParaSeguimiento(this.listaIdHijos, this.idacceso).subscribe(
          (res) => {
            /* tslint:disable:no-string-literal */
            if (res['status'] === 200) {
              this.empleados = res['body'][0];
              this.observaciones = res['body'][1];
            } else {
              /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData = new PopupInformativoModel('Error', message, 'danger');
              const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
            }
            /* tslint:enable:no-string-literal */
          }
        );
      }
    }

    this.ckeConfig = {
      allowedContent: false,
      extraPlugins: 'divarea',
      skin: 'kama',
      forcePasteAsPlainText: true,
      // tslint:disable-next-line: max-line-length
  //    removeButtons: 'Source,Save,NewPage,Templates,Form,Checkbox,Radio,TextField,Textarea,Select,Button,ImageButton,HiddenField,CreateDiv,Language,Anchor,CreatePlaceholder,Image,Flash,PageBreak,Iframe,InsertPre,Maximize,ShowBlocks,oembed,MediaEmbed',
      removePlugins: 'about,elementspath'
      // resize_enabled: false
    };
    // Toolbar configuration generated automatically by the editor based on config.toolbarGroups.
    this.ckeConfig.toolbar = [
      { name: 'document', groups: [ 'mode', 'document', 'doctools' ], items: [ 'Preview', 'Print' ] },
      // tslint:disable-next-line: max-line-length
      { name: 'clipboard', groups: [ 'clipboard', 'undo' ], items: [ 'Cut', 'Copy', 'Paste', 'PasteText', 'PasteFromWord', '-', 'Undo', 'Redo' ] },
      { name: 'editing', groups: [ 'find', 'selection', 'spellchecker' ], items: [ 'Find', 'Replace', '-', 'SelectAll', '-', 'Scayt' ] },
      // tslint:disable-next-line: max-line-length
      { name: 'basicstyles', groups: [ 'basicstyles', 'cleanup' ], items: [ 'Bold', 'Italic', 'Underline', 'Strike', 'Subscript', 'Superscript', '-', 'CopyFormatting', 'RemoveFormat' ] },
      // tslint:disable-next-line: max-line-length
      { name: 'paragraph', groups: [ 'list', 'indent', 'blocks', 'align', 'bidi' ], items: [ 'NumberedList', 'BulletedList', '-', 'Outdent', 'Indent', '-', 'Blockquote', '-', 'JustifyLeft', 'JustifyCenter', 'JustifyRight', 'JustifyBlock', '-', 'BidiLtr', 'BidiRtl' ] },
      // '/',
      { name: 'styles', items: [ 'Styles', 'Format', 'Font', 'FontSize' ] },
      { name: 'colors', items: [ 'TextColor', 'BGColor' ] },
      { name: 'links', items: [ 'Link', 'Unlink' ] },
      { name: 'insert', items: [ 'Table', 'HorizontalRule', 'Smiley', 'SpecialChar' ] }
    ];

    // Toolbar groups configuration.
    this.ckeConfig.toolbarGroups = [
      { name: 'document', groups: [ 'mode', 'document', 'doctools' ] },
      { name: 'clipboard', groups: [ 'clipboard', 'undo' ] },
      { name: 'editing', groups: [ 'find', 'selection', 'spellchecker' ] },
      { name: 'basicstyles', groups: [ 'basicstyles', 'cleanup' ] },
      { name: 'paragraph', groups: [ 'list', 'indent', 'blocks', 'align', 'bidi' ] },
      // '/',
      { name: 'styles' },
      { name: 'colors' },
      { name: 'links' },
      { name: 'insert' }
    ];
  }

  // convenience getter for easy access to form fields
  get f() { return this.correoForm.controls; }

  //#region CKEditor
  onChange($event: any): void {
    // console.log('onChange');
    // this.log += new Date() + "<br />";
  }

  onPaste($event: any): void {
    // console.log('onPaste');
    // this.log += new Date() + "<br />";
  }
  //#endregion CKEditor

  getErrorMessage() {
    return this.correoForm.get('asuntoCtrl').hasError('required') ? 'Este campo es obligatorio' :
        this.correoForm.get('asuntoCtrl').hasError('minlength') ? 'El texto ingresado es corto' :
            '';
  }

  // actuliza la opción de broadcast en función al cambio en el checkbox
  onChangeBroadcast(event) {
    this.broadcast = event.checked;
    // cargo todas las areas en el chipSelectedArea
    if (this.broadcast) {
      this.chipSelectedAreas = this.todasLasAreas;
    } else {
      this.chipSelectedAreas = [];
    }
  }

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

  // función para verificar si tiene permiso para hacer seguimientos
  puedeHacerSeguimiento() {
    switch (this.idacceso) {
      case 1:
      case 3:
      case 9:
      case 10:
      case 21:
      case 18:
      case 22:
      case 13:
        this.permisoSeguimiento = true;
        break;
      default:
        this.permisoSeguimiento = false;
    }
  }

  public resetSelects() {
    // clear input element
    // this.areasInput.nativeElement.value = '';
    if (!this.f.seguimientoCtrl.value) {
      // clear control value and trigger Ctrl.valueChanges event
      this.f.calificacionesCtrl.setValue(false);
      this.f.observacionCtrl.setValue(false);
      this.f.empleadoAobservarCtrl.setValue(false);
    }
  }

  adjuntarArchivos(fileInput: any) {
    // this.misArchivos = fileInput.target.files as File[];
    // this.misArchivos = Array.from(this.misArchivos);
    const files = fileInput.target.files as File[];
/*     let tamanioMaximo = 0;
    for (const file of files) {
      tamanioMaximo += file.size;
    }
    if (tamanioMaximo > x) {
      error tamaño maximo global excedido
    } else {
      hago lo de abajo aca adentro
    } */
    for (const file of files) {
      if (file.size < 5000000) { // 5Mb
        if (this.misArchivos === null || this.misArchivos === []) {
          this.misArchivos = [file];
        } else {
          this.misArchivos.push(file);
        }
      }
    }
  }

  removerArchivo(fileInput: any): void {
    const index = this.misArchivos.indexOf(fileInput);
    if (index >= 0) {
      this.misArchivos.splice(index, 1);
    }
  }

  removerArchivoReenvio(adj: any): void {
    const index = this.adjuntosReenvio.indexOf(adj);
    if (index >= 0) {
      this.adjuntosReenvio.splice(index, 1);
    }
  }

  // this is used to trigger the input
  openInput() {
    // your can use ElementRef for this later
    document.getElementById('fileInput').click();
  }

  deshabilitarEnvio(): boolean {
    if (this.ruta !== 'responder') {
      let seguimientoOk;
      if (this.f.seguimientoCtrl.value) {
        // pregunta si algun campo de seguimiento esta incompleto
        // tslint:disable-next-line: max-line-length
        if (this.f.calificacionesCtrl.value === false || this.f.observacionCtrl.value === false || this.f.empleadoAobservarCtrl.value === false) {
          seguimientoOk = false;
        } else {
          seguimientoOk = true;
        }
      } else {
        seguimientoOk = true;
      }
      // tslint:disable-next-line: max-line-length
      if ((this.chipSelectedUsuarios.length <= 0 && this.chipSelectedAreas.length <= 0) || this.correoForm.invalid || seguimientoOk === false) {
        return true;
      } else {
        this.puedoDesactivar = false;
        return false;
      }
    } else {
      // TODO: hacer el deshabilitar para responder
      if (this.correoForm.invalid || this.destinatarios.length <= 0 || this.detalleAresponder.asunto !== '' ) {
        this.puedoDesactivar = false;
        return false;
      } else {
        return true;
      }
    }
  }

  enviar() {
    const message = `Está seguro de que quiere enviar el correo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.puedoDesactivar = true;
        if (this.ruta !== 'responder') {
          this.spinner = true;
          // tslint:disable-next-line: max-line-length
          this.destinatarios = UtilesMensajeria.generarDestinatarios(this.chipSelectedUsuarios, this.chipSelectedAreas, this.todosLosUsuarios);
          const data = {
            username: this.authService.currentUserValue.username,
            // token: this.authService.currentUserValue.token,
            idusuario: this.authService.currentUserValue.id,
            asunto: this.f.asuntoCtrl.value,
            cuerpo: UtilesMensajeria.decodificarString(this.f.cuerpoCtrl.value),
            area: this.authService.currentUserValue.idarea,
            destinos: this.destinatarios,
            seguimiento: false,
            tipoobservacion: 0,
            calificacion: '',
            sector: 0,
            idempleado: 0,
            cuerpo2: '',
            reenviar: false,
            adjuntosReenvio: []
          };

          if (this.adjuntosReenvio.length > 0) {
            data.reenviar = true;
            data.adjuntosReenvio = this.adjuntosReenvio;
          }

          if (this.f.seguimientoCtrl.value === false) {
            data.seguimiento = false;
          } else {
            data.seguimiento = true;
            data.tipoobservacion = this.f.observacionCtrl.value.id;
            data.calificacion = this.f.calificacionesCtrl.value.valor;
            data.sector = this.f.empleadoAobservarCtrl.value.sector;
            data.idempleado = this.f.empleadoAobservarCtrl.value.id;
            // tslint:disable-next-line: max-line-length
            data.cuerpo2 = data.cuerpo + '       Seguimiento efectuado a: ' + this.f.empleadoAobservarCtrl.value.nombre + '   Tipo: ' + this.f.observacionCtrl.value.nombre + ' ( ' + this.f.calificacionesCtrl.value.detalle + ' )';
          }

          const formData = new FormData();
          formData.append('data', JSON.stringify(data));
          if (this.misArchivos === null) {
            formData.append('cantidad', '0');
          } else {
            formData.append('cantidad', this.misArchivos.length.toString());
            for (let i = 0; i < this.misArchivos.length; i++) {
              formData.append('adjunto' + i, this.misArchivos[i]);
            }
          }
          this.mensajeriaService.postNuevoCorreo(formData).subscribe(
            (res) => {
              this.spinner = false;
              // tslint:disable-next-line: no-string-literal
              if (res['status'] === 200) {
                this.router.navigate(['/mensajeria/bandeja-de-entrada']);
              } else {
                const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
                const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
                const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});
              }
            }
          );
        } else {
          this.spinner = true;
          let creoAlOriginal;
          if (this.detalleAresponder.remitente === this.authService.currentUserValue.id) {
            creoAlOriginal = true;
          } else {
            creoAlOriginal = false;
          }
          const data = {
            username: this.authService.currentUserValue.username,
            idusuario: this.authService.currentUserValue.id,
            cuerpo: UtilesMensajeria.decodificarString(this.f.cuerpoCtrl.value),
            area: this.authService.currentUserValue.idarea,
            fueQuienLoGenero: creoAlOriginal,
            respuestas: this.detalleAresponder.respuestas,
            idmensaje: this.idmensaje
          };

          const formData = new FormData();
          formData.append('data', JSON.stringify(data));
          if (this.misArchivos === null) {
            formData.append('cantidad', '0');
          } else {
            formData.append('cantidad', this.misArchivos.length.toString());
            for (let i = 0; i < this.misArchivos.length; i++) {
              formData.append('adjunto' + i, this.misArchivos[i]);
            }
          }
          this.mensajeriaService.postRespuesta(formData).subscribe(
            (res) => {
              this.spinner = false;
              // tslint:disable-next-line: no-string-literal
              if (res['status'] === 200) {
                this.router.navigate(['/mensajeria/bandeja-de-entrada']);
              } else {
                /*const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
                const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
                const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});*/
              }
            }
          );
        }
      }
    });
  }

  canDeactivate(): Observable<boolean> | boolean {
    if (this.puedoDesactivar) {
      return true;
    } else {
      // console.log('misArchivos.length: %o', this.misArchivos.length);
      switch (this.ruta) {
        case 'responder':
          if (this.correoForm.dirty || this.correoForm.touched) {
            const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
            return of(confirmation);
          }
          return true;
          break;

        case 'reenviar':
          // tslint:disable-next-line: max-line-length
          if (this.correoForm.dirty || this.correoForm.touched || this.chipSelectedUsuarios.length > 0
            || this.chipSelectedAreas.length > 0 ) {
            const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
            return of(confirmation);
          }
          return true;
          break;

        default:
          // tslint:disable-next-line: max-line-length
          if ( this.chipSelectedUsuarios.length > 0 || this.chipSelectedAreas.length > 0
            || this.correoForm.dirty || this.correoForm.touched) { // || this.misArchivos.length > 0 ) {
            // const message = `Está seguro de que quiere descartar los cambios?`;
            // const dialogData = new ConfirmDialogModel('Confirmar', message);
            // const dialogRef = this.dialog.open(DialogoConfirmacionComponent, dialogConfig);
            // dialogRef.afterClosed().subscribe(result => {
            //   return of(result);
            // });
            // TODO: mejorar y usar un modal
            const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
            return of(confirmation);
          }
          return true;
          break;
      }
    }
  }

  test() {
    const original = '<p>otra respuesta par ver car&aacute;cter raro</p>';
    console.log('Original: %o', this.f.cuerpoCtrl.value);
    console.log('Deco: %o', UtilesMensajeria.decodificarString(this.f.cuerpoCtrl.value));
  }

  volver() {
    this.router.navigate(['/mensajeria/bandeja-de-entrada/' + this.idmensaje]);
  }

  // TODO: corregir canDeactive
}
