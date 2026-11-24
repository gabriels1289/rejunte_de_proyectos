import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { AdminComponent } from './admin/admin.component';
import { AdminDashboardComponent } from './admin-dashboard/admin-dashboard.component';
import { GestionEmpleadosComponent } from './gestion-empleados/gestion-empleados.component';
import { GestionReportesComponent } from './gestion-reportes/gestion-reportes.component';

import { AuthGuard } from '../auth/auth.guard';
import { Rol } from '../_models/roles';

const adminRoutes: Routes = [
  {
    path: '',
    component: AdminComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: '',
        canActivateChild: [AuthGuard],
        children: [
          { path: 'reportes', component: GestionReportesComponent },
          { path: 'empleados', component: GestionEmpleadosComponent, data: { roles: [ Rol.ControlDeJuegoIII ]} },
          { path: '', component: AdminDashboardComponent }
        ]
      }
    ]
  }
];

@NgModule({
  imports: [RouterModule.forChild(adminRoutes)],
  exports: [RouterModule]
})
export class AdminRoutingModule { }
