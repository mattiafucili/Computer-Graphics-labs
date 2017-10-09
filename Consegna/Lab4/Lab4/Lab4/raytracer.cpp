#include "raytracer.h"
#include "material.h"
#include "vectors.h"
#include "argparser.h"
#include "raytree.h"
#include "utils.h"
#include "mesh.h"
#include "face.h"
#include "sphere.h"

// casts a single ray through the scene geometry and finds the closest hit
bool
RayTracer::CastRay (Ray & ray, Hit & h, bool use_sphere_patches) const
{
    bool answer = false;
    Hit nearest;
    nearest = Hit();
    
    // intersect each of the quads
    for (int i = 0; i < mesh->numQuadFaces (); i++)
    {
        Face *f = mesh->getFace (i);
        if (f->intersect (ray, h, args->intersect_backfacing))
        {
            if( h.getT() < nearest.getT() )
            {
                answer = true;
                nearest = h;
            }
        }
    }
    
    // intersect each of the spheres (either the patches, or the original spheres)
    if (use_sphere_patches)
    {
        for (int i = mesh->numQuadFaces (); i < mesh->numFaces (); i++)
        {
            Face *f = mesh->getFace (i);
            if (f->intersect (ray, h, args->intersect_backfacing))
            {
                if( h.getT() < nearest.getT() )
                {
                    answer = true;
                    nearest = h;
                }
            }
        }
    }
    else
    {
        const vector < Sphere * >&spheres = mesh->getSpheres ();
        for (unsigned int i = 0; i < spheres.size (); i++)
        {
            if (spheres[i]->intersect (ray, h))
            {
                if( h.getT() < nearest.getT() )
                {
                    answer = true;
                    nearest = h;
                }
            }
        }
    }
    
    h = nearest;
    return answer;
}


Vec3f
RayTracer::Shadow(Vec3f pointOnLight, Vec3f normal, Face & f, Vec3f answer, Material & m, Ray ray, Hit hit, Vec3f point) const {
    Ray *n_ray;
    Hit *new_hit;
    Vec3f n_point, dista;
    
    Vec3f dirToLight = pointOnLight - point;
    dirToLight.Normalize();
    
    n_ray = new Ray(point, dirToLight);
    
    new_hit = new Hit();
    
    bool colpito = CastRay(*n_ray, *new_hit, false);
    
    if(colpito) {
        n_point = n_ray -> pointAtParameter(new_hit -> getT());
        dista.Sub(dista, n_point, pointOnLight);
        
        if(dista.Length() < 0.01) {
            if (normal.Dot3 (dirToLight) > 0) {
                Vec3f lightColor = 0.2 * f.getMaterial()->getEmittedColor() * f.getArea();
                answer += m.Shade (ray, hit, dirToLight, lightColor, args); // Phong
            }
        }
    }
    return answer;
}

Vec3f
RayTracer::TraceRay (Ray & ray, Hit & hit, int bounce_count) const {
    hit = Hit ();
    bool intersect = CastRay (ray, hit, false);
    
    if( bounce_count == args->num_bounces )
        RayTree::SetMainSegment (ray, 0, hit.getT () );
    else
        RayTree::AddReflectedSegment(ray, 0, hit.getT() );
    
    Vec3f answer = args->background_color;
    
    Material *m = hit.getMaterial ();
    if (intersect == true) {
        assert (m != NULL);
        Vec3f normal = hit.getNormal ();
        Vec3f point = ray.pointAtParameter (hit.getT ());
        
        // ----------------------------------------------
        // ambient light
        answer = args->ambient_light * m->getDiffuseColor ();
        
        // ----------------------------------------------
        // add each light
        int num_lights = mesh->getLights().size();
        int shadowSamples = args->num_shadow_samples;
        Vec3f shadowAnswer;
        
        for (int i = 0; i < num_lights; i++) {
            // ==========================================
            // ASSIGNMENT:  ADD SHADOW LOGIC
            // ==========================================
            Face *f = mesh->getLights()[i];
            
            Vec3f pointOnLight;
            if(shadowSamples == 1) {
                pointOnLight = f->computeCentroid();
                answer = Shadow(pointOnLight, normal, *f, answer, *m, ray, hit, point);
            } else {
                for(int j = 0; j < shadowSamples; j++) {
                    pointOnLight = f->RandomPoint();
                    shadowAnswer = Shadow(pointOnLight, normal, *f, shadowAnswer, *m, ray, hit, point);
                }
                shadowAnswer /= shadowSamples;
            }
        }
        if(shadowSamples > 1)
            answer += shadowAnswer;
        
        // ----------------------------------------------
        // if the surface is shiny...
        Vec3f reflectiveColor = m->getReflectiveColor ();
        
        if(reflectiveColor.Length() != 0 && bounce_count > 0) {
            Vec3f rayVector = ray.getOrigin() - point;
            Vec3f reflection = (2 * normal.Dot3(rayVector) * normal) - rayVector;
            reflection.Normalize();
            Ray ray = Ray(point, reflection);
            answer += TraceRay(ray, hit, bounce_count - 1);
            
            answer = answer * reflectiveColor;
        }
    }
    
    return answer;
}
