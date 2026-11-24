import { Component, OnInit } from '@angular/core';
import { Router, ActivatedRoute, ParamMap, NavigationStart } from '@angular/router';

import { AuthService } from 'src/app/auth/auth.service';

@Component({
  selector: 'app-empleado-detalle',
  templateUrl: './empleado-detalle.component.html',
  styleUrls: ['./empleado-detalle.component.css']
})
export class EmpleadoDetalleComponent implements OnInit {

  private idempleado;
  public nombre: string;
  public apellido: string;
  private foto: string;
  sector: string;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private authService: AuthService
    ) {
      // este metodo solo es valido en el constructor
      // console.log(this.router.getCurrentNavigation().extras.state);
      this.idempleado = this.rutaActiva.snapshot.params.idempleado;
      this.nombre = this.rutaActiva.snapshot.params.nombre;
      this.apellido = this.rutaActiva.snapshot.params.apellido;
      this.sector = this.rutaActiva.snapshot.params.sector;

      /* this.nombre = this.rutaActiva.snapshot.queryParamMap.get('nombre') || '';
      this.apellido = this.rutaActiva.snapshot.queryParamMap.get('apellido') || '';
      this.foto = this.rutaActiva.snapshot.queryParamMap.get('foto') || '';
      this.sector = 'AUX. SUP. GASTRONOMIA'; */
    }

  ngOnInit() {

    //#region comentarios
    /* this.empleado$ = this.rutaActiva.paramMap.pipe(
      switchMap((params: ParamMap) =>
        this.service.getEmpleado(params.get('id')))
    );*/
    // console.log('History: %o', history.state);
    // this.empleado = history.state;
    //#endregion
  }

}

