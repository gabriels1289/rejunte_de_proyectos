import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { EvaluacionesComponent } from './evaluaciones/evaluaciones.component';
import { AuthGuard } from '../auth/auth.guard';
import { ListadoComponent } from './listado/listado.component';
import { CrearEvaluacionComponent } from './crear-evaluacion/crear-evaluacion.component';
import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';
// import { DetalleComponent } from './detalle/detalle.component';
import { EvaluarComponent } from './evaluar/evaluar.component';
import { RankingYEdicionComponent } from './ranking-y-edicion/ranking-y-edicion.component';
import { ComparacionComponent } from './comparacion/comparacion.component';
import { RankingGlobalComponent } from './ranking-global/ranking-global.component';
import { MenuComponent } from './menu/menu.component';
import { MisEvaluacionesComponent } from './mis-evaluaciones/mis-evaluaciones.component';

const evaluacionesRoutes: Routes = [
  {
    path: 'evaluaciones',
    component: EvaluacionesComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: '',
        canActivateChild: [AuthGuard],
        children: [
          { path: '', component: ListadoComponent },
          { path: 'crear-evaluacion', component: CrearEvaluacionComponent, canDeactivate: [CanDeactivateGuard] },
          { path:  ':idevaluacion/:titulo', component: MenuComponent,
            children: [
              { path: 'evaluar', component: EvaluarComponent, canDeactivate: [CanDeactivateGuard] },
              { path: 'ranking-y-edicion', component: RankingYEdicionComponent },
              { path: 'comparacion', component: ComparacionComponent },
              { path: 'ranking-global', component: RankingGlobalComponent }
            ]
          }
        ]
      }
    ]
  },
  { path: 'mis-evaluaciones', component: MisEvaluacionesComponent, canActivate: [AuthGuard] }
];

@NgModule({
  imports: [RouterModule.forChild(evaluacionesRoutes)],
  exports: [RouterModule]
})
export class EvaluacionesRoutingModule { }
