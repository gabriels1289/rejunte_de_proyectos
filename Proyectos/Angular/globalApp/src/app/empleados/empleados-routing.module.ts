import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { ListadoComponent } from './listado/listado.component';
import { EmpleadoDetalleComponent } from './empleado-detalle/empleado-detalle.component';
import { EmpleadosComponent } from './empleados/empleados.component';

import { AuthGuard } from '../auth/auth.guard';
import { Rol } from '../_models/roles';
import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';
import { DatosPersonalesComponent } from './datos-personales/datos-personales.component';
import { SeguimientosComponent } from './seguimientos/seguimientos.component';
import { EvaluacionesComponent } from './evaluaciones/evaluaciones.component';
import { FormularioComponent } from './formulario/formulario.component';

const empleadosRoutes: Routes = [
  {
    path: 'empleados',
    component: EmpleadosComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: '',
        canActivateChild: [AuthGuard],
        children: [
          { path: '', component: ListadoComponent },
          { path: 'nuevo-empleado', component: FormularioComponent, canDeactivate: [CanDeactivateGuard] },
          { path: ':idempleado/:nombre/:apellido/:idsector/:sector', component: EmpleadoDetalleComponent,
            children: [
              { path: 'datos-personales', component: DatosPersonalesComponent, canDeactivate: [CanDeactivateGuard] },
              { path: 'seguimientos', component: SeguimientosComponent },
              { path: 'evaluaciones', component: EvaluacionesComponent },
            ]
          },
        ]
      }
    ]
  }
];

@NgModule({
  imports: [RouterModule.forChild(empleadosRoutes)],
  exports: [RouterModule]
})
export class EmpleadosRoutingModule { }
