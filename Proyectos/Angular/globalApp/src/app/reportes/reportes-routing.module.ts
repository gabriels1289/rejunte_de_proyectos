import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { ReportesListaComponent } from './reportes-lista/reportes-lista.component';
import { ReporteDetalleComponent } from './reporte-detalle/reporte-detalle.component';
import { ReportesHomeComponent } from './reportes-home/reportes-home.component';
import { ReportesComponent } from './reportes/reportes.component';

import { AuthGuard } from '../auth/auth.guard';

import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';
import { ReporteDetalleResolverService } from './reporte-detalle-resolver.service';

const reportesRoutes: Routes = [
  {
    path: '',
    canActivate: [AuthGuard],
    component: ReportesComponent,
    children: [
      {
        path: '',
        component: ReportesListaComponent,
        canActivateChild: [AuthGuard],
        children: [
          {
            path: ':id',
            component: ReporteDetalleComponent,
            canDeactivate: [CanDeactivateGuard],
            resolve: {
              reporte: ReporteDetalleResolverService
            }
          },
          {
            path: '',
            component: ReportesHomeComponent
          }
        ]
      }
    ]
  }
];

@NgModule({
  imports: [RouterModule.forChild(reportesRoutes)],
  exports: [RouterModule]
})
export class ReportesRoutingModule { }
