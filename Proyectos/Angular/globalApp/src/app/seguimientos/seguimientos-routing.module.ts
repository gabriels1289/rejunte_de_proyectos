import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { AuthGuard } from '../auth/auth.guard';
import { Rol } from '../_models/roles';
import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';
import { SeguimientosComponent } from './seguimientos/seguimientos.component';

const seguimientosRoutes: Routes = [
  { path: 'seguimientos', component: SeguimientosComponent, canActivate: [AuthGuard] },
];

@NgModule({
  imports: [RouterModule.forChild(seguimientosRoutes)],
  exports: [RouterModule]
})
export class SeguimientosRoutingModule { }
