import { Component, OnInit, OnDestroy } from '@angular/core';
import { MensajeriaService } from '../servicios/mensajeria.service';

import { ActivatedRoute, Params, Router } from '@angular/router';
import { Correo, UtilesMensajeria } from '../comunes/correo';
import { Observable, Subscription, interval } from 'rxjs';

import { PageEvent } from '@angular/material/paginator';
import { MatDialog } from '@angular/material';
import { ModalEntradaComponent } from '../modales/modal-entrada/modal-entrada.component';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-bandeja-de-entrada-respuestas',
  templateUrl: './bandeja-de-entrada-respuestas.component.html',
  styleUrls: ['./bandeja-de-entrada-respuestas.component.css']
})
export class BandejaDeEntradaRespuestasComponent implements OnInit, OnDestroy {

  // la agregue por el error del compilador
  public $last;
  // la agregue por el error del compilador
  public last;

  public spinner = true;
  private respuestas$: Observable<Correo>;
  private idmensaje: number;
  public detalleMensaje: Correo; // detalle del mensaje
  public destinatarios;  // propiedad con los destinatarios
  public textoDestinatarios = ''; // propiedad con la concatenacion de los nombres de los destinatarios
  public adjuntos;
  public todasLasRespuestas: Correo[]; // pasaje para poder utilizar el metodo slice en la paginación
  selectedId: number;
  private lecturas;
  private tabla;
  private idusuario;

  // MatPaginator Inputs
  pagina = 0; // guarda el numero de pagina de donde se llegó
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  subscription: Subscription;

  constructor(
    private mensajeriaService: MensajeriaService,
    private rutaActiva: ActivatedRoute,
    private router: Router,
    public dialog: MatDialog
  ) { }

  ngOnInit() {
    this.idmensaje = this.rutaActiva.snapshot.params.idmensaje;
    this.pagina = +this.rutaActiva.snapshot.queryParamMap.get('pagina') || 0;

    this.rutaActiva.queryParamMap.subscribe(
      params => {
        this.pagina = +params.get('pagina') || 0;
        this.lecturas = +params.get('lecturas') || 0;
        this.tabla = params.get('tabla');
        this.idusuario = +params.get('idusuario');
      }
    );

    this.rutaActiva.params.subscribe(
      (params: Params) => {
        this.idmensaje = params.idmensaje;
      }
    );

    this.obtenerRespuestas();
    const fuenteDeTiempo = interval(900000); // 15 minutos
    this.subscription = fuenteDeTiempo.subscribe(val => this.obtenerRespuestas());
  }

  ngOnDestroy() {
    this.subscription.unsubscribe();
  }

  obtenerRespuestas() {
    this.mensajeriaService.getBandejaDeEntradaRespuestas(this.idmensaje, this.lecturas, this.tabla, this.idusuario).subscribe(
      (res) => {
        // console.log(res);
        /* tslint:disable:no-string-literal */
        if (res['status'] === 200) {
          this.detalleMensaje = res['body'][0];
          this.destinatarios = res['body'][1];
          this.textoDestinatarios = UtilesMensajeria.concatenarDestinatarios(this.destinatarios);
          this.adjuntos = res['body'][2];
          if (res['body'][3] !== 'consulta vacia') {
            this.respuestas$ = res['body'][3];
            this.todasLasRespuestas = this.respuestas$ as unknown as Correo[]; // hago este pasaje para poder utilizar el metodo slice
            for (const respu of this.todasLasRespuestas) {
              respu.cuerpo2 = UtilesMensajeria.acomodarString(respu.cuerpo);
            }
          }
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
          // TODO: ver como manejar los errores
        }
        /* tslint:enable:no-string-literal */
      }
    );
  }

  // función para setear el tamaño de la paginación
  setPageSizeOptions(setPageSizeOptionsInput: string) {
    this.pageSizeOptions = setPageSizeOptionsInput.split(',').map(str => +str);
  }

  public handlePage(e: PageEvent): PageEvent {
    this.pageSize = e.pageSize;
    this.pageIndex = e.pageIndex;
    return e;
  }

  openDialog(respuesta: Correo) {
    this.selectedId = respuesta.id;
    this.dialog.open(ModalEntradaComponent, {data: respuesta});
  }

  /* private concatenarDestinatarios() {
    if (this.destinatarios !== 'consulta vacia') {
      for (const dest of this.destinatarios) {
        this.textoDestinatarios += (dest.area + ', ');
      }
    }
  } */

  goToBandejaDeEntrada(correo: Correo) {
    const correoId = correo ? correo.id : null;
    // Paso el Id del correo actual a la bandeja de entrada si es que está disponible como obligatorio (router param)
    // para resaltar el mensaje con un estilo de CSS
    // paso como opcional (query param) el numero de página donde estaba el correo
    // this.router.navigate(['/mensajeria/bandeja-de-entrada', { id: correoId }],
    // { queryParams: { idcorreo: correoId, pagina: this.pagina }});
    this.router.navigate(['/mensajeria/bandeja-de-entrada'], { queryParams: { idcorreo: correoId, pagina: this.pagina }});
  }

  public goToReenviar(correo: Correo) {
    this.router.navigate(['/mensajeria/reenviar'], { queryParams: { idmensaje: correo.id, tabla: 'respuestas' }});
  }

  public goToResponder() {
    this.router.navigate(['mensajeria/responder', this.idmensaje]);
  }

}
