import { Component, OnInit, OnDestroy } from '@angular/core';
import { MensajeriaService } from '../servicios/mensajeria.service';
import { ActivatedRoute, Router } from '@angular/router';

import { Observable, interval, Subscription } from 'rxjs';

import { Correo } from '../comunes/correo';
import { AuthService } from '../../auth/auth.service';

import { PageEvent } from '@angular/material/paginator';
import { SearchService } from 'src/app/utilidades/search/search.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { MatDialog } from '@angular/material';

@Component({
  selector: 'app-bandeja-de-entrada',
  templateUrl: './bandeja-de-entrada.component.html',
  styleUrls: ['./bandeja-de-entrada.component.css']
})
export class BandejaDeEntradaComponent implements OnInit, OnDestroy {
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor

  private terminoDeBusqueda;

  private correos$: Observable<Correo[]>;
  public todosLosCorreos: Correo[]; // pasaje para poder utilizar el metodo slice en la paginación
  selectedId: number;

  // la agregue por el error del compilador
  public last;

  // MatPaginator Inputs
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  idusuario: number;

  subscription: Subscription;

  constructor(
    private mensajeriaService: MensajeriaService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    private searchService: SearchService,
    public dialog: MatDialog
    ) { }

  ngOnInit() {
    this.idusuario = this.authService.currentUserValue.id;

    /* this.selectedId = +this.rutaActiva.snapshot.queryParamMap.get('idcorreo') || 0;
    this.pageIndex = +this.rutaActiva.snapshot.queryParamMap.get('pagina') || 0;
 */
    this.rutaActiva.queryParamMap.subscribe(
      params => {
        this.pageIndex = +params.get('pagina') || 0;
        this.selectedId = +params.get('idcorreo') || 0;
      }
    );
    this.obtenerCorreos();
    const fuenteDeTiempo = interval(900000); // 15 minutos
    this.subscription = fuenteDeTiempo.subscribe(val => this.obtenerCorreos());
  }

  ngOnDestroy() {
    this.subscription.unsubscribe();
  }

  obtenerCorreos() {
    this.mensajeriaService.getBandejaDeEntrada(this.idusuario).subscribe(
      (res) => {
        // console.log(res);
        // console.log(res['status']);
        // console.log('codigo de estado: ' + res['status']);
        // console.log('texto de estado: ' + res['statusText']);
        // console.log('body: %o', res['body']);
        // this.length = res['body'].length;
        // tslint:disable-next-line: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          // tslint:disable-next-line: no-string-literal
          this.correos$ = res['body'];
          this.todosLosCorreos = this.correos$ as unknown as Correo[]; // hago este pasaje para poder utilizar el metodo slice
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
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

  public goToRespuestas(correo: Correo) {
    // veo quien lo creo y selecciono la tabla destino
    let auxTabla;
    let numeroLecturas;
    if (correo.remitente === this.authService.currentUserValue.id) {
      auxTabla = 'mensajes';
    } else {
      auxTabla = 'area_mensaje';
    }
    // si no esta leido incremento en la cantidad de lecturas
    if (correo.leido === 'N') {
      numeroLecturas = ++correo.lecturas;
    } else {
      numeroLecturas = correo.lecturas;
    }
    this.router.navigate(['/mensajeria/bandeja-de-entrada', correo.id],
    { queryParams: { pagina: this.pageIndex, lecturas: numeroLecturas, tabla: auxTabla, idusuario: this.authService.currentUserValue.id }});
  }

  onBusca(terminoDeBusqueda: string) {
    this.terminoDeBusqueda = terminoDeBusqueda;
    console.log('terminoDeBusqueda band: %o', terminoDeBusqueda);
    console.log('Por evento: %o', this.searchService.search2(terminoDeBusqueda, this.todosLosCorreos, 'asunto'));
  }

  public goToReenviar(correo: Correo) {
    this.router.navigate(['/mensajeria/reenviar'], { queryParams: { idmensaje: correo.id, tabla: 'mensajes' }});
  }

  // TODO: hacer bigBrother
}
