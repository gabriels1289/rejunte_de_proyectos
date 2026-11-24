import { Injectable } from '@angular/core';
import { Router, Resolve, RouterStateSnapshot, ActivatedRouteSnapshot } from '@angular/router';
import { Observable, of, EMPTY } from 'rxjs';
import { mergeMap, take } from 'rxjs/operators';

import { ReporteService } from './reporte.service';
import { Reporte } from './reporte';

@Injectable({
  providedIn: 'root'
})
export class ReporteDetalleResolverService implements Resolve<Reporte> {

  constructor(private rs: ReporteService, private router: Router) { }

  resolve(route: ActivatedRouteSnapshot, state: RouterStateSnapshot): Observable<Reporte> | Observable<never> {
    const id = route.paramMap.get('id');

    return this.rs.getReporte(id).pipe(
      take(1),
      mergeMap(reporte => {
        if (reporte) {
          return of(reporte);
        } else {  // id not found
          this.router.navigate(['/reportes']);
          return EMPTY;
        }
      })
    );
  }
}
