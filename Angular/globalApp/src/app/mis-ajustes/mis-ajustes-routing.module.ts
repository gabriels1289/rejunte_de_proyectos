import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PerfilComponent } from './perfil/perfil.component';
import { AuthGuard } from '../auth/auth.guard';
import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';
import { LogsComponent } from './logs/logs.component';


const misAjustesRoutes: Routes = [
  { path: 'perfil', component: PerfilComponent, canActivate: [AuthGuard], canDeactivate: [CanDeactivateGuard]},
  { path: 'logs', component: LogsComponent, canActivate: [AuthGuard]}
];

@NgModule({
  imports: [RouterModule.forChild(misAjustesRoutes)],
  exports: [RouterModule]
})
export class MisAjustesRoutingModule { }
